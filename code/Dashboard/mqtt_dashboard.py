import paho.mqtt.client as mqtt
import pandas as pd
from datetime import datetime, timedelta
import dash
from dash import dcc, html
from dash.dependencies import Input, Output
import plotly.graph_objs as go
from plotly.subplots import make_subplots
import threading
from collections import deque
import dash_bootstrap_components as dbc

# cache to store the data points
data_cache = deque(maxlen=1000000000)


broker = "iotwx.ucar.edu"
port = 1883
topic = "/test/topic"

# Callback function to handle incoming messages
def on_message(client, userdata, msg):
    payload = msg.payload.decode()
    parsed_data = parse_payload(payload)
    if parsed_data:
        parsed_data['timestamp'] = datetime.now()
        data_cache.append(parsed_data)

def parse_payload(payload):
    lines = payload.split('\n')
    parsed_data = {}
    for line in lines:
        if line.startswith('device:'):
            parsed_data['device_id'] = line.split(':')[1].strip()
        elif line.startswith('sensor:'):
            parsed_data['sensor_type'] = line.split(':')[1].strip()
        elif line.startswith('m:'):
            parsed_data['measurement'] = float(line.split(':')[1].strip())
        elif line.startswith('rssi:'):
            parsed_data['rssi'] = int(line.split(':')[1].strip())
    return parsed_data

client = mqtt.Client()
client.on_message = on_message
client.connect(broker, port, 60)
client.subscribe(topic)


mqtt_thread = threading.Thread(target=client.loop_forever)
mqtt_thread.start()

# CSS styling
custom_css = """
@keyframes fadeIn {
    from { opacity: 0; }
    to { opacity: 1; }
}
.fade-in {
    animation: fadeIn 0.5s ease-in-out;
}
.custom-card {
    border-radius: 15px;
    box-shadow: 0 4px 6px rgba(0, 0, 0, 0.1);
    transition: all 0.3s ease;
}
.custom-card:hover {
    transform: translateY(-5px);
    box-shadow: 0 6px 8px rgba(0, 0, 0, 0.15);
}
"""

# Initialize Dash app 
app = dash.Dash(__name__, external_stylesheets=[dbc.themes.FLATLY, 'https://fonts.googleapis.com/css2?family=Poppins:wght@300;400;600&display=swap'])

# layout of the dashboard
app.layout = html.Div([
    html.Link(rel='stylesheet', href='https://use.fontawesome.com/releases/v5.8.1/css/all.css'),
    # Include custom CSS using dcc.Store
    dcc.Store(id='custom-css', data={'css': custom_css}),
    dbc.Container([
        dbc.Row([
            dbc.Col(html.Img(src="/assets/placeholder.png", height="100px"), width=4),
            dbc.Col(html.H1("Live MQTT Sensor Dashboard", className="text-primary mb-4"), width=6)
        ], className="align-items-center my-4"),
        dbc.Card([
            dbc.CardBody([
                dbc.Row([
                    dbc.Col(
                        dcc.Dropdown(
                            id='device-dropdown',
                            options=[
                                {'label': 'Device: adafruit/rp2040/D65417536', 'value': 'adafruit/rp2040/D65417536'},
                                {'label': 'Device: adafruit/rp2040/D65417121', 'value': 'adafruit/rp2040/D65417121'}
                            ],
                            value='adafruit/rp2040/D65417536',
                            style={'width': '100%'}
                        ),
                        width=6,
                        className="mx-auto"
                    )
                ], className="mb-4"),
                dbc.Row([
                    dbc.Col(html.Div(id='average-readings', className="fade-in"), className="text-center mb-4")
                ]),
                dbc.Row([
                    dbc.Col(dcc.Graph(id='live-graph', animate=True, className="fade-in"))
                ])
            ])
        ], className="custom-card"),
        dcc.Interval(
            id='interval-component',
            interval=30*1000,
            n_intervals=0
        )
    ], fluid=True)
])

# Custom CSS
app.clientside_callback(
    """
    function(css_data) {
        const style = document.createElement('style');
        style.textContent = css_data.css;
        document.head.appendChild(style);
        return '';
    }
    """,
    Output('custom-css', 'data'),
    Input('custom-css', 'data')
)

@app.callback(
    [Output('live-graph', 'figure'),
     Output('average-readings', 'children')],
    [Input('device-dropdown', 'value'),
     Input('interval-component', 'n_intervals')]
)
def update_graph(selected_device, n_intervals):
    df = pd.DataFrame(list(data_cache))
    
    if df.empty:
        return go.Figure(), "No data available"
    
    device_data = df[df['device_id'] == selected_device]
    device_data = device_data[device_data['timestamp'] > datetime.now() - timedelta(minutes=60)]
    
    sensor_types = sorted(device_data['sensor_type'].unique(), reverse=True)
    
    fig = make_subplots(rows=len(sensor_types), cols=1, 
                        subplot_titles=[st for st in sensor_types],
                        shared_xaxes=True, vertical_spacing=0.02)
    
    average_readings = []
    
    for i, sensor in enumerate(sensor_types, start=1):
        sensor_data = device_data[device_data['sensor_type'] == sensor]
        fig.add_trace(
            go.Scatter(x=sensor_data['timestamp'], y=sensor_data['measurement'],
                       mode='lines+markers', name=sensor,
                       line=dict(width=3), marker=dict(size=8)),
            row=i, col=1
        )
        
        fig.update_yaxes(title_text="Measurement", row=i, col=1)
        
        avg_reading = sensor_data['measurement'].tail(10).mean() if len(sensor_data) >= 10 else sensor_data['measurement'].mean()
        average_readings.append(
            dbc.Card(
                dbc.CardBody([
                    html.H5(sensor, className="card-title"),
                    html.P(f"{avg_reading:.2f}", className="card-text")
                ], className="mb-2 text-white bg-dark")
            )
        )
    
    fig.update_layout(
        height=200 * len(sensor_types),
        title_text=f"Sensor Measurements for {selected_device}",
        showlegend=False,
        hovermode="x unified",
        template="none",
        font=dict(family="Poppins, sans-serif"),
        plot_bgcolor='rgba(0,0,0,0)',
        paper_bgcolor='rgba(0,0,0,0)'
    )
    fig.update_xaxes(title_text="Time", row=len(sensor_types), col=1)
    
    average_readings_div = html.Div([
        html.H3("Average Readings (Last 10 Measurements)", className="text-primary mb-3"),
        dbc.Row([dbc.Col(card, width=3) for card in average_readings], justify="center")
    ])
    
    return fig, average_readings_div

# JavaScript to reload the page, so that data is refreshed
app.clientside_callback(
    """
    function(n_intervals) {
        if (n_intervals > 0) {
            location.reload();
        }
        return null;
    }
    """,
    Output('interval-component', 'n_intervals'),
    Input('interval-component', 'n_intervals')
)


if __name__ == '__main__':
    app.run_server(debug=True)
