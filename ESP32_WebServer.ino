#include <WiFi.h>
#include <ESPAsyncWebServer.h>

AsyncWebServer server(80);

// Wi-Fi information
const char* ssid = "Osolemio";
const char* password = "bonbonchocolatelac4315";

// Create static IP
// IPAddress local_IP(192, 168, 1, 184);  // IP Address
// IPAddress gateway(192, 168, 1, 1);     // Gateway
// IPAddress subnet(255, 255, 255, 0);    // Subnet Mask
// IPAddress primaryDNS(8, 8, 8, 8);      // primary DNS
// IPAddress secondaryDNS(8, 8, 4, 4);    // DNS

float temperature = 0.0;
float maxTemp = 0.0;
float minTemp = 40.0;
float humidity = 0.0;
float lightIntensity = 0;
float pressure = 0;
bool pauseReading = false;

// HTML for Web server
const char htmlContent[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">

<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>ESP32 Web Server</title>
    <link rel="stylesheet" href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/css/bootstrap.min.css">
    <link rel='stylesheet' href='https://cdnjs.cloudflare.com/ajax/libs/font-awesome/6.0.0-beta3/css/all.min.css'>
    <script src="https://cdn.jsdelivr.net/npm/@popperjs/core@2.11.6/dist/umd/popper.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0-alpha1/dist/js/bootstrap.min.js"></script>
    <script src="https://cdn.jsdelivr.net/npm/echarts/dist/echarts.min.js"></script>
    <script>
        function handleButtonClick(event, url) {
            event.preventDefault();
            var xhr = new XMLHttpRequest();
            xhr.open("GET", url, true);

            xhr.onload = function () {
                if (xhr.status === 200) {
                    console.log("Response:", xhr.responseText);
                } else {
                    console.error("Error:", xhr.statusText);
                }
            };

            xhr.onerror = function () {
                console.error("Request failed");
            };

            xhr.send();
        }
    </script>
</head>

<body class="d-flex justify-content-evenly" style="font-size: 18px;background-color: #dde9f5;">
    <div class="container col-md-10 col-12 m-3 d-flex flex-wrap justify-content-between gap-2"
        style="color: #0d47a1; font-family: serif;">
        <div class="text-center col-12 col-md-9 fw-bold d-flex justify-content-center align-items-center"
            style="background-color: #0d47a1; color: white; border-radius: 10px; min-height: 70px;">
            <h1 class="m-0">REPORT STATION</h1>
        </div>

        <div class="text-center col-12 col-md-2 fw-bold d-flex flex-wrap justify-content-center"
            style="font-family: 'Courier New', monospace; border: 2px solid #0d47a1;border-radius: 10px; background-color: white; color: #0d47a1;">
            <div class="col-6 col-md-10 text-md-center text-end p-2 border-md-bottom" id="dateClock">mm dd</div>
            <div class="col-6 col-md-10 text-md-center text-start p-2" id="timeClock">hh:mm:ss</div>
        </div>
        <div class="current col-md-5 col-12 d-flex flex-wrap justify-content-between"
            style="border-radius: 10px; background-color: white; overflow-x: auto;">
            <div class="col-12">
                <h3 class="text-center"
                    style="background-color: #3f51b5; color: white; border-radius: 10px 10px 0 0;">Current data</h3>
            </div>
            <div class="col-12 text-center d-flex border-bottom">
                <p class="col-12 display-4 fw-bold p-2">
                    <i class="fas fa-thermometer-half"></i><span id="temperature">25.89</span><span class="display-6"
                        style="vertical-align: text-top;">°C</span>
                </p>
            </div>
            <div class="col-4 text-center py-2">
                <i class="fas fa-tint"></i> <span id="humidity">80.73</span>%
            </div>
            <div class="col-4 text-center py-2">
                <i class="fas fa-lightbulb"></i> <span id="lightIntensity">800</span> lux
            </div>
            <div class="col-4 text-center py-2">
                <i class="fas fa-tachometer-alt"></i> <span id="pressure">1800</span> hPa
            </div>
        </div>
        <div class="average col-md-6 col-12" style="border-radius: 10px; background-color: white;">
            <div class="col-12">
                <h3 class="text-center"
                    style="background-color: #3f51b5; color: white; border-radius: 10px 10px 0 0;">Statistics</h3>
            </div>
            <div class="row border-bottom">
                <div class="col-4  d-none d-md-block">
                </div>
                <div class="col text-md-start text-center">
                    Max
                </div>
                <div class="col text-md-start text-center">
                    Average
                </div>
                <div class="col text-md-start text-center">
                    Min
                </div>
                <div class="col text-center">
                    Unit
                </div>
            </div>
            <div class="row">
                <div class="col-4 d-none d-md-block">
                    <p class="ps-2 fst-italic">Temperature</p>
                </div>
                <div class="col text-md-start text-center">
                    <span id="maxTemp">30.66</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="avgTemp">00.00</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="minTemp">00.36</span>
                </div>
                <div class="col text-center">
                    °C
                </div>
            </div>
            <div class="row">
                <div class="col-4 d-none d-md-block">
                    <p class="ps-2 fst-italic">Humidity</p>
                </div>
                <div class="col text-md-start text-center">
                    <span id="maxHum">30.66</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="avgHum">00.00</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="minHum">00.36</span>
                </div>
                <div class="col text-center">
                    %
                </div>
            </div>
            <div class="row">
                <div class="col-4 d-none d-md-block">
                    <p class="ps-2 fst-italic">Light Intensity</p>
                </div>
                <div class="col text-md-start text-center">
                    <span id="maxLight">30.66</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="avgLight">00.00</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="minLight">00.36</span>
                </div>
                <div class="col text-center">
                    lux
                </div>
            </div>
            <div class="row">
                <div class="col-4 d-none d-md-block">
                    <p class="ps-2 fst-italic">Pressure</p>
                </div>
                <div class="col text-md-start text-center">
                    <span id="maxPress">30.66</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="avgPress">00.00</span>
                </div>
                <div class="col text-md-start text-center">
                    <span id="minPress">00.36</span>
                </div>
                <div class="col text-center">
                    hPa
                </div>
            </div>
        </div>
        <div class="chart col-12 d-flex flex-wrap justify-content-evenly gap-2"
            style="border-radius: 10px; background-color: white;">
            <div class="col-12">
                <h3 class="text-center"
                    style="background-color: #3f51b5; color: white; border-radius: 10px 10px 0 0;">Overview</h3>
            </div>
            <div class="col-11 col-md-5">
                <canvas id="lineChart"></canvas>
            </div>
            <div class="col-11 col-md-6">
                <canvas id="bubbleChart" style="width:100%; margin: 0 auto;"></canvas>
            </div>
        </div>
        <div class="button-container col-12 d-flex flex-wrap justify-content-center gap-2 mt-2">
            <button id="pauseButton" class="btn btn-outline-primary col-4"
                onclick="handleButtonClick(event, '/pause')">Pause</button>
            <button id="resumeButton" class="btn btn-outline-primary col-4"
                onclick="handleButtonClick(event, '/resume')">Resume</button>
        </div>
        <div class="footer col-12 text-center mt-2" style="font-family: Verdana, sans-serif; color: gray;">
            <p>&copy; 2024 LogicDesignProject L03N08. All rights reserved.</p>
        </div>
    </div>

    <script src="https://cdn.jsdelivr.net/npm/chart.js"></script> <!-- Thư viện Chart.js -->
    <script>
        document.addEventListener('DOMContentLoaded', (event) => {
            const ctxTempHumidity = document.getElementById('lineChart').getContext('2d');
            const tempHumidityChart = new Chart(ctxTempHumidity, {
                type: 'line',
                data: {
                    labels: ['1', '2', '3', '4', '5', '6', '7', '8', '9', '10'],
                    datasets: [{
                        label: 'Temperature (°C)',
                        data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0], // Temperature data (update real time)
                        borderColor: 'rgba(255, 99, 132, 1)',
                        backgroundColor: 'rgba(255, 99, 132, 0.2)',
                        yAxisID: 'y1', // Gán cho trục Y1
                    }, {
                        label: 'Humidity (%)',
                        data: [0, 0, 0, 0, 0, 0, 0, 0, 0, 0], // humidity data (update real time)
                        borderColor: 'rgba(54, 162, 235, 1)',
                        backgroundColor: 'rgba(54, 162, 235, 0.2)',
                        yAxisID: 'y2',
                    }]
                },
                options: {
                    responsive: true,
                    scales: {
                        y1: {
                            type: 'linear',
                            position: 'left',
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: 'Temperature (°C)'
                            },
                        },
                        y2: {
                            type: 'linear',
                            position: 'right',
                            beginAtZero: true,
                            title: {
                                display: true,
                                text: 'Humidity (%)'
                            },
                            grid: {
                                drawOnChartArea: false // Loại bỏ lưới cho trục Y2
                            }
                        }
                    }
                }
            });

            let bubbleData = [
                { x: 75, y: 29, r: 18, lightIntensity: 100 },
                { x: 50, y: 20, r: 10, lightIntensity: 900 }
            ];

            var ctx = document.getElementById('bubbleChart').getContext('2d');
            var myBubbleChart = new Chart(ctx, {
                type: 'bubble',
                data: {
                    datasets: [{
                        label: 'Environment Data',
                        data: bubbleData.map(d => ({
                            x: d.x,
                            y: d.y,
                            r: d.r / 90,
                            lightIntensity: d.lightIntensity
                        })),
                        backgroundColor: function (context) {
                            const lightIntensity = context.raw.lightIntensity || 800;
                            const intensityRatio = Math.min(lightIntensity / 1500, 1);
                            const blueValue = Math.floor(255 - 255 * intensityRatio);
                            return `rgb(0, ${blueValue}, 255)`;
                        },
                        borderWidth: 1,
                        borderColor: 'rgba(0, 0, 0, 0.1)'
                    }]
                },
                options: {
                    scales: {
                        y: {
                            title: {
                                display: true,
                                text: 'Temperature (°C)'
                            },
                            beginAtZero: true,
                            min: 0,
                            max: 50
                        },
                        x: {
                            title: {
                                display: true,
                                text: 'Humidity (%)'
                            },
                            beginAtZero: true,
                            min: 0,
                            max: 100
                        }
                    },
                    plugins: {
                        tooltip: {
                            callbacks: {
                                label: function (context) {
                                    const data = context.raw;
                                    return `Temperature: ${data.y}°C\nHumidity: ${data.x}%\nPressure: ${data.r * 90} hPa\nLight Intensity: ${data.lightIntensity} lux`;
                                }
                            }
                        },
                        beforeDraw: function (chart) {
                            const ctx = chart.ctx;
                            ctx.save();
                            chart.data.datasets[0].data.forEach((dataPoint, index) => {
                                const x = chart.scales.x.getPixelForValue(dataPoint.x);
                                const y = chart.scales.y.getPixelForValue(dataPoint.y);
                                ctx.fillStyle = 'black';
                                ctx.fillText(`#${index + 1}`, x, y);
                            });
                            ctx.restore();
                        }
                    }

                }
            });

            function updateBubbleChart(newData) {
                bubbleData.push(newData);

                if (bubbleData.length > 10) {
                    bubbleData.shift();
                }

                myBubbleChart.data.datasets[0].data = bubbleData.map(d => ({
                    x: d.x,
                    y: d.y,
                    r: d.r / 90,
                    lightIntensity: d.lightIntensity
                }));
                myBubbleChart.update();
            }

            function calAvg(data) {
                const total = {
                    temperature: 0,
                    humidity: 0,
                    pressure: 0,
                    lightIntensity: 0
                };

                data.forEach(d => {
                    total.temperature += d.y;
                    total.humidity += d.x;
                    total.pressure += d.r;
                    total.lightIntensity += d.lightIntensity;
                });

                const averageCount = data.length;
                return {
                    temp: (total.temperature / averageCount).toFixed(2),
                    hum: (total.humidity / averageCount).toFixed(2),
                    press: (total.pressure / averageCount).toFixed(2),
                    light: (total.lightIntensity / averageCount).toFixed(2)
                };
            }

            function calMax(data) {
                const max = {
                    temp: 0,
                    hum: 0,
                    press: 0,
                    light: 0
                };

                data.forEach(d => {
                    if (max.temp < d.y) max.temp = d.y;
                    if (max.hum < d.x) max.hum = d.x;
                    if (max.press < d.r) max.press = d.r;
                    if (max.light < d.lightIntensity) max.light = d.lightIntensity;
                });

                return max;
            }

            function calMin(data) {
                const min = {
                    temp: 150,
                    hum: 150,
                    press: 2000,
                    light: 2000
                };

                data.forEach(d => {
                    if (min.temp > d.y) min.temp = d.y;
                    if (min.hum > d.x) min.hum = d.x;
                    if (min.press > d.r) min.press = d.r;
                    if (min.light > d.lightIntensity) min.light = d.lightIntensity;
                });

                return min;
            }


            function updateData() {
                fetch('/data')
                    .then(response => response.json())
                    .then(data => {
                        document.getElementById('temperature').innerText = data.temperature;
                        document.getElementById('humidity').innerText = data.humidity;
                        document.getElementById('lightIntensity').innerText = data.lightIntensity;
                        document.getElementById('pressure').innerText = data.pressure;

                        tempHumidityChart.data.datasets[0].data.shift();
                        tempHumidityChart.data.datasets[0].data.push(data.temperature);
                        tempHumidityChart.data.datasets[1].data.shift();
                        tempHumidityChart.data.datasets[1].data.push(data.humidity);
                        tempHumidityChart.update();

                        updateBubbleChart({
                            x: data.humidity,
                            y: data.temperature,
                            r: data.pressure,
                            lightIntensity: data.lightIntensity
                        });

                        let max = calMax(bubbleData);
                        document.getElementById('maxTemp').innerText = max.temp;
                        document.getElementById('maxHum').innerText = max.hum;
                        document.getElementById('maxLight').innerText = max.light;
                        document.getElementById('maxPress').innerText = max.press;

                        let avg = calAvg(bubbleData);
                        document.getElementById('avgTemp').innerText = avg.temp;
                        document.getElementById('avgHum').innerText = avg.hum;
                        document.getElementById('avgLight').innerText = avg.light;
                        document.getElementById('avgPress').innerText = avg.press;

                        let min = calMin(bubbleData);
                        document.getElementById('minTemp').innerText = min.temp;
                        document.getElementById('minHum').innerText = min.hum;
                        document.getElementById('minLight').innerText = min.light;
                        document.getElementById('minPress').innerText = min.press;
                    });
            }

            let updateInterval = 3000;
            let updateTimer;
            let updating = true;

            function startUpdating() {
                if (updating) {
                    updating = true;
                    updateTimer = setInterval(updateData, updateInterval);
                }
            }

            function pauseUpdating() {
                updating = false;
                clearInterval(updateTimer);
            }

            const pauseButton = document.getElementById('pauseButton');
            const resumeButton = document.getElementById('resumeButton');

            pauseButton.addEventListener('click', () => {
                updating = false;
                pauseButton.disabled = true;
                resumeButton.disabled = false;
                pauseUpdating();
            });

            resumeButton.addEventListener('click', () => {
                updating = true;
                resumeButton.disabled = true;
                pauseButton.disabled = false;
                startUpdating();
            });

            updateTimer = setInterval(updateData, updateInterval);

            function formatTime12Hour(hours, minutes, seconds) {
                const period = hours >= 12 ? 'PM' : 'AM';
                hours = hours % 12 || 12;
                return `${hours}:${minutes.toString().padStart(2, '0')}:${seconds} ${period}`;
            }

            function updateTime() {
                const now = new Date();

                const day = now.getDate();
                const month = now.toLocaleString('en-US', { month: 'long' });
                const hours = now.getHours();
                const minutes = now.getMinutes();
                const seconds = String(now.getSeconds()).padStart(2, '0');

                const formattedTime = formatTime12Hour(hours, minutes, seconds);
                const formattedDate = `${month} ${day}`;

                document.getElementById('timeClock').innerText = formattedTime;
                document.getElementById('dateClock').innerText = formattedDate;
            }

            setInterval(updateTime, 1000);
        });
    </script>
</body>

</html>
)rawliteral";

void handlePause(AsyncWebServerRequest *request) {
    pauseReading = true;
    Serial.println("Sensors are PAUSED reading.");
    Serial1.print('p');
    request->send(200, "application/json", "{\"status\": \"paused\"}");
}

void handleResume(AsyncWebServerRequest *request) {
    pauseReading = false;
    Serial.println("Sensors are READING...");
    Serial1.print('r');
    request->send(200, "application/json", "{\"status\": \"resumed\"}");
}

void setup() {
    Serial.begin(115200);                       // UART0
    Serial1.begin(115200, SERIAL_8N1, 16, 17);  // UART1 (RX=16, TX=17)

    // Create static IP address
    // if (!WiFi.config(local_IP, gateway, subnet, primaryDNS, secondaryDNS)) {
    //     Serial.println("Fail to create static IP address!");
    // }

    // Wi-Fi connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Wi-Fi connecting...");
    }
    Serial.println("Wi-Fi connected!");
    Serial.println(WiFi.localIP());  // print IP address

    server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
        request->send(200, "text/html", htmlContent);
    });

    server.on("/pause", HTTP_GET, handlePause);
    server.on("/resume", HTTP_GET, handleResume);

    server.on("/data", HTTP_GET, [](AsyncWebServerRequest* request) {
        String jsonResponse = "{\"temperature\": " + String(temperature) + 
                      ", \"maxTemp\": " + String(maxTemp) + 
                      ", \"minTemp\": " + String(minTemp) + 
                      ", \"humidity\": " + String(humidity) + 
                      ", \"lightIntensity\": " + String(lightIntensity) + 
                      ", \"pressure\": " + String(pressure) + "}";

        request->send(200, "application/json", jsonResponse);
    });
    server.begin();
}

String getValue(String data, char separator, int index) {
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;
    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
        found++;
        strIndex[0] = strIndex[1] + 1;
        strIndex[1] = (i == maxIndex) ? i + 1 : i;
        }
    }
    return (found > index) ? data.substring(strIndex[0], strIndex[1]) : "";
}

void loop() {
    if (!pauseReading) Serial1.print('r');
    else Serial1.print('p');
    // Read data from STM32 through UART1
    if (Serial1.available()) {
        String data = Serial1.readStringUntil('\n'); 

        if (data.length() > 0) {
            // Extract data
            temperature = getValue(data, ';', 0).toFloat();
            humidity = getValue(data, ';', 1).toFloat();
            lightIntensity = getValue(data, ';', 2).toInt();
            pressure = getValue(data, ';', 3).toInt();
            if (temperature > maxTemp) maxTemp = temperature;
            if (temperature < minTemp) minTemp = temperature;

            // Send message to STM32
            const char *ackMessage = "Data received\n";
            Serial1.print(ackMessage);

            // Print to check on Serial Monitor
            if (pauseReading) Serial.println("STM32: " + data);
            Serial.println("Data received from STM32: " + String(temperature) + " " + String(humidity) + " " + String(lightIntensity) + " " + String(pressure));
        } else {
            // Send message error to STM32
            const char *errorMessage = "Data not received\n";
            Serial1.print(errorMessage);
        }
    }
    // delay(20);
}
