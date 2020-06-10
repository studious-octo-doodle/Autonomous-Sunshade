/***************************Methods below this line*************************/

/******************networkedServer() method********************/
/*
   This method is used to serve a website after the user's
   network parameters have been successfully loaded to the
   arduino's flash memory.
*/
void networkedServer() {
  WiFiClient client = server.available();   // listen for incoming clients
  if (client) {                             // if you get a client,
    Serial.println("new client");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        //Serial.println(c);
        //Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE html>"
                           "<html lang=\"en\" >"
                           "<head>"
                           "<meta charset=\"UTF-8\">"
                           "<title>It&#39;s Always Sunny In Arizona</title>"
                           "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><link rel='stylesheet' href='https://netdna.bootstrapcdn.com/bootstrap/3.1.0/css/bootstrap.css'><link rel=\"stylesheet\" href=\"http://72.14.190.151/style.css\">"

                           "</head>"
                           "<body>"
                           "<!-- partial:index.partial.html -->"
                           "<div class=\"container-fluid\" ng-app=\"tabApp\">"
                           "<div class=\"row row-collapse\" ng-controller=\"TabController\">"
                           "<div class=\"col-md-2 nav-collapse\">"
                           "<ul class=\"top-nav nav nav-pills nav-stacked\">"
                           "<li ng-class=\"{ active: isSet(1) }\">"
                           "<a href ng-click=\"setTab(1)\">Home</a>"
                           "</li>"
                           "<li ng-class=\"{ active: isSet(2) }\">"
                           "<a href ng-click=\"setTab(2)\">Status Report</a>"
                           "</li>"
                           "<li ng-class=\"{ active: isSet(3) }\">"
                           "<a href ng-click=\"setTab(3)\">Automated Settings</a>"
                           "</li>"
                           "</ul>"
                           "</div>"
                           "<div class=\"col-md-8\">"
                           "<div class=\"jumbotron\">"
                           "<div ng-show=\"isSet(1)\">"
                           "<h1>It's Always Sunny In Arizona</h1>"
                           "<p>Welcome to the website!</p>"
                           "<p><a class=\"btn btn-up btn-lg btn-shade\" role=\"button\" href='/UP'>Raise Shade</a>"
                           "<a class=\"btn btn-down btn-lg btn-shade\" role=\"button\" href='/DOWN'>Lower Shade</a> </p>"
                           "<div class=\"cards\">"
                           "<div class=\"card temp\">"
                           "<div class=\"inner\">"
                           "<div class=\"icon\"></div>"
                           "<div class=\"title\">"
                           "<div class=\"text\">TEMPERATURE</div>"
                           "</div>"
                           "<div class=\"number\">");
                           client.print(String(thisWindSensor.tempC));
                           client.println("</div>"
                           "<div class=\"measure\">CELCIUS</div>"
                           "</div>"
                           "</div>"
                           "<div class=\"card energy\">"
                           "<div class=\"inner\">"
                           "<div class=\"icon\"></div>"
                           "<div class=\"title\">"
                           "<div class=\"text\">Wind Speed</div>"
                           "</div>"
                           "<div class=\"number\">");
                           client.print(String(thisWindSensor.windMPH));
                           client.println("</div>"
                           "<div class=\"measure\">MPH</div>"
                           "</div>"
                           "</div>"
                           "</div>"
                           "<p style=\"font-size:36px;\">Weather Forecast</p>");
                           client.print(printWeatherForecast());
                           client.println("</div>"
                           "<div ng-show=\"isSet(2)\">"
                           "<h1>Status Report</h1>");
                           client.print(printReport());
                           client.println("</div>"
                           "<div ng-show=\"isSet(3)\">"
                           "<h1> Automated Control Settings</h1>"
                           "<div class=\"input-option\">"
                           "<div class=\"option-badge\">"
                           "<label class=\"option-label\" for=\\\"lightControl\\\">Control Options</label>"
                           "</div>"
                           "<div class=\"option-body\">"
                           "<select id=\"control\" name=\"control\" form=\"settings\">"
                           "<option value=\"1\">Automate based on temperature</option>"
                           "<option value=\"2\">Automate based on light</option>"
                           "<option value=\"3\">Automate based on sunrise/sunset</option>"
                           "<option value=\"0\">Automation Off</option>"
                           "</select>"
                           "</div>"
                           "</div>"
                           "<div class=\"input-option\">"
                           "<div class=\"option-badge\">"
                           "<label class=\"option-label\" for=\"tempLower\">Temperature to Lower</label>"
                           "</div>"
                           "<div class=\"option-body\">"
                           "<select id=\"tempLower\" name=\"tempLower\" form=\"settings\">"
                           "<option value=\"20\">20 &#176 C</option>"
                           "<option value=\"25\">25 &#176 C</option>"
                           "<option value=\"30\">30 &#176 C</option>"
                           "<option value=\"35\">35 &#176 C</option>"
                           "</select>"
                           "</div>"
                           "</div>"
                           "<div class=\"input-option\">"
                           "<div class=\"option-badge\">"
                           "<label class=\"option-label\" for=\\\"tempRaise\\\">Temperature to Raise</label>"
                           "</div>"
                           "<div class=\"option-body\">"
                           "<select id=\"tempRaise\" name=\"tempRaise\" form=\"settings\">"
                           "<option value=\"0\">0 &#176 C</option>"
                           "<option value=\"5\">5 &#176 C</option>"
                           "<option value=\"10\">10 &#176 C</option>"
                           "<option value=\"15\">15 &#176 C</option>"
                           "</select>"
                           "</div>"
                           "</div>"
                           "<form action=\"/\" id=\"settings\">"
                           "<input class=\"auto-form-submit\" type=\"submit\" \>"
                           "</form>"
                           "</div>"
                           "</div>"
                           "</div>"
                           "</div>"
                           "<!-- partial -->"
                           "<script src='https://cdnjs.cloudflare.com/ajax/libs/jquery/2.1.3/jquery.min.js'></script>"
                           "<script src='https://cdnjs.cloudflare.com/ajax/libs/angular.js/1.2.10/angular.min.js'></script><script  src=\"http://72.14.190.151/script.js\"></script>"

                           "</body>"
                           "</html>");


            /*
             * OLD WEBSITE
                          "<html>"
                                       "<head>"
                                       "<style>"
                                       "body {  background-color: #8C1D40;}"
                                       "h1 { color: #FFC627; text-align: center;}"
                                       "caption,td {  color: #FFC627; text-align: center; font-weight: bold; font-family: verdana;  font-size: 20px;}"
                                       "label { font-family: verdana; font-size: 20px; padding: 20px;}"
                                       "div {margin: 10px;}"
                                       ".config {"
                                       "margin: auto;"
                                       "padding: 10px;"
                                       "}"
                                       ".button {"
                                       "background-color: #e7e7e7;"
                                       "border: none;"
                                       "color: black;"
                                       "padding: 15px 32px;"
                                       "text-align: center;"
                                       "text-decoration: none;"
                                       "display: inline-block;"
                                       "font-size: 16px;"
                                       "margin: 4px 2px;"
                                       "cursor: pointer;"
                                       "width: 50%;"
                                       "height: 50%;"
                                       "border-radius: 12px;"
                                       "box-shadow: 4px 4px 2px black;"
                                       "}"
                                       "</style>"
                                       "</head>"
                                       "<body>"
                                       "<table width=\"100%\">"
                                       "<tr>"
                                       "<td colspan=\"2\">"
                                       "<canvas id=\"myCanvas\" width=\"600\" height=\"75\"></canvas>"
                                       "</td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td width=\"50%\" align=\"center\">"
                                       "<canvas id=\"temp\" width=\"400\" height=\"100\"></canvas>"
                                       "</td>"
                                       "<td width=\"50%\" align=\"center\">"
                                       "<canvas id=\"wind\" width=\"400\" height=\"100\"></canvas>"
                                       "</td>"
                                       "</tr>"
                                       "<tr>"
                                       "<th> Temperature (&#176 C)</th>"
                                       "<th> Wind Speed (MPH)</th>"
                                       "</tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\"><hr/><span id=\"datetime\"></span></td>"
                                       "</tr>"
                                       "<tr><td>");
                        client.print(weatherArray[0].timeDescription);
                        client.println("</td><td>");
                        client.print(weatherArray[1].timeDescription);
                        client.println("</td></tr><tr><td>");
                        client.print(weatherArray[0].condition);
                        client.println("</td><td>");
                        client.print(weatherArray[1].condition);
                        client.println("</td></tr><tr><td>Wind ");
                        client.print(weatherArray[0].wind);
                        client.println(" MPH</td><td>Wind ");
                        client.print(weatherArray[1].wind);
                        client.println(" MPH</td></tr><tr><td>");
                        client.print(weatherArray[0].temp);
                        client.println("&#176 C</td><td>");
                        client.print(weatherArray[1].temp);
                        client.println("&#176 C</td></tr><td colspan=\"2\" align=\"center\"><hr/>Sunrise: ");
                        client.print(stringTime(sunrise) + "     Sunset: " + stringTime(sunset));
                        client.println("</td>"
                                       "<tr height=\"50px\"></tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\"><hr/></td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\">Shade Controls</td>"
                                       "</tr>"
                                       "<tr height=\"50px\"></tr>"
                                       "<tr>"
                                       "<td width=\"50%\" align=\"center\"><button class=\"button\" type=\"button\" onclick=\"window.location.href = '/UP';\">Raise Shade</button></td>"
                                       "<td width=\"50%\" align=\"center\"><button class=\"button\" type=\"button\" onclick=\"window.location.href = '/DOWN';\">Lower Shade</button></td>"
                                       "</tr>"
                                       "<tr height=\"50px\"></tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\"><hr/></td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\">Automated Control Settings</td>"
                                       "</tr>"
                                       "<tr height=\"50px\"></tr>"
                                       "<tr>"
                                       "<td><label for=\"lightControl\">Control Options:</label></td>"
                                       "<td>"
                                       "<select id=\"control\" name=\"control\" form=\"settings\">"
                                       "<option value=\"1\">Automate based on temperature</option>"
                                       "<option value=\"2\">Automate based on light</option>"
                                       "<option value=\"3\">Automate based on sunrise/sunset</option>"
                                       "<option value=\"0\">Automation Off</option>"
                                       "</select>"
                                       "</td>"
                                       "</tr>"
                                       "<tr height=\"25px\"></tr>"
                                       "<tr>"
                                       "<td><label for=\"tempLower\">Temperature to Lower:</label></td>"
                                       "<td>"
                                       "<select id=\"tempLower\" name=\"tempLower\" form=\"settings\">"
                                       "<option value=\"20\">20 &#176 C</option>"
                                       "<option value=\"25\">25 &#176 C</option>"
                                       "<option value=\"30\">30 &#176 C</option>"
                                       "<option value=\"35\">35 &#176 C</option>"
                                       "</select>"
                                       "</td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td><label for=\"tempRaise\">Temperature to Raise:</label></td>"
                                       "<td>"
                                       "<select id=\"tempRaise\" name=\"tempRaise\" form=\"settings\">"
                                       "<option value=\"0\">0 &#176 C</option>"
                                       "<option value=\"5\">5 &#176 C</option>"
                                       "<option value=\"10\">10 &#176 C</option>"
                                       "<option value=\"15\">15 &#176 C</option>"
                                       "</select>"
                                       "</td>"
                                       "</tr>"
                                       "<tr>"
                                       "<td colspan=\"2\" align=\"center\">"
                                       "<form action=\"/\" id=\"settings\">"
                                       "<input type=\"submit\">"
                                       "</form>"
                                       "</td></tr><tr><td colspan=\"2\" align=\"center\"><hr/></td>"
                                       "</tr><tr><td colspan=\"2\" align=\"center\">Status Report</td></tr>"
                                       "<tr><td colspan=\"2\" align=\"center\">");
                                       client.print(printReport());
                                       client.println("</td></tr>"
                                       "<tr height=\"100px\"></tr>"
                                       "<tr>"
                                       "<td colspan=\"2\">Thank you for choosing products made by It's Always Sunny In Arizona</td>"
                                       "</tr>"
                                       "</table>"
                                       "<script>"
                                       "function setTime(){"
                                       "var dt = new Date();"
                                       "document.getElementById(\"datetime\").innerHTML = dt.toLocaleString();"
                                       "}"
                                       "setInterval(setTime,1000);"
                                       "var canvas = document.getElementById('myCanvas');"
                                       "var context = canvas.getContext('2d');"
                                       "context.font = \"20pt verdana\";"
                                       "context.shadowOffsetX = 3;"
                                       "context.shadowOffsetY = 3;"
                                       "context.textAlign = \"center\";"
                                       "context.fillStyle = \"#FFC627\";"
                                       "context.shadowColor = \"rgba(0,0,0,1)\";"
                                       "context.shadowBlur = 4;"
                                       "context.fillText(\"It's Always Sunny In Arizona\", canvas.width/2, canvas.height/2);"
                                       "var canvasTemp = document.getElementById('temp');"
                                       "var contextTemp = canvasTemp.getContext('2d');"
                                       "contextTemp.font = \"70pt verdana\";"
                                       "contextTemp.shadowOffsetX = 3;"
                                       "contextTemp.shadowOffsetY = 3;"
                                       "contextTemp.textAlign = \"center\";"
                                       "contextTemp.fillStyle = \"#32a6a8\";"
                                       "contextTemp.shadowColor = \"rgba(0,0,0,1)\";"
                                       "contextTemp.shadowBlur = 4;"
                                       "contextTemp.fillText(");
                        client.print("\"" + String(thisWindSensor.tempC) + "\"");
                        client.print(",canvasTemp.width/2, canvasTemp.height-20);"
                                     "var canvasWind = document.getElementById('wind');"
                                     "var contextWind = canvasWind.getContext('2d');"
                                     "contextWind.font = \"80pt verdana\";"
                                     "contextWind.shadowOffsetX = 3;"
                                     "contextWind.shadowOffsetY = 3;"
                                     "contextWind.textAlign = \"center\";"
                                     "contextWind.fillStyle = \"#32a6a8\";"
                                     "contextWind.shadowColor = \"rgba(0,0,0,1)\";"
                                     "contextWind.shadowBlur = 4;"
                                     "contextWind.fillText(");
                        client.print("\"" + String(thisWindSensor.windMPH) + "\"");
                        client.print(",canvasWind.width/2, canvasWind.height-20);"
                                     "</script>"
                                     "<footer>"
                                     "<p>Version: 11.0 </p>"
                                     "</footer>"
                                     "</body>"
                                     "</html>");*/
                                     
                        // the content of the HTTP response follows the header:
                        // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          else {      // if you got a newline, then clear currentLine:
            if (responseCount <= 9) {
              responseArray[responseCount] = currentLine;
              responseCount ++;
            }
            currentLine = "";
          }
        }
        else if (c != '\r') {    // if you got anything else but a carriage return character,
          currentLine += c;      // add it to the end of the currentLine
        }

        //Serial.println(currentLine);
        // Check to see if the client request was "GET /UP" or "GET /DOWN":

      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");

    if (responseArray[0].startsWith("GET /?control")) {
      String thisString = responseArray[0];
      int controlStartIndex = thisString.indexOf("?") + 9;
      int controlEndIndex = thisString.indexOf("&");
      int tempLowerStartIndex = thisString.indexOf("&") + 11;
      int tempLowerEndIndex = thisString.indexOf("&", tempLowerStartIndex);
      int tempRaiseStartIndex =  tempLowerEndIndex + 11;
      int tempRaiseEndIndex = thisString.length() - 9;
      String controlString = thisString.substring(controlStartIndex, controlEndIndex);
      String tempLowerString = thisString.substring(tempLowerStartIndex, tempLowerEndIndex);
      String tempRaiseString = thisString.substring(tempRaiseStartIndex, tempRaiseEndIndex);
      //Serial.println(controlString);
      //Serial.println(tempLowerString);
      //Serial.println(tempRaiseString);
      switch (controlString.toInt()) {
        case 1:
          controlSetting = 1; //1 = temperature
          Serial.println("Control = temperature");
          break;
        case 2:
          controlSetting = 2; //2 = light
          Serial.println("Control = light");
          break;
        case 3:
          controlSetting = 3; //sun
          Serial.println("Control = sun");
          break;
        default:
          controlSetting = 0; //0 = none
          Serial.println("Control = manual");
          break;
      }
      switch (tempRaiseString.toInt()) {
        case 0:
          tempToRaise = 0; // 0 degrees C
          Serial.println("Raise Temp = 0" );
          break;
        case 5:
          tempToRaise = 5; // 5 degrees C
          Serial.println("Raise Temp = 5");
          break;
        case 10:
          tempToRaise = 10;// 10 degrees C
          Serial.println("Raise Temp = 10");
          break;
        default:
          tempToRaise = 15;// 15 degrees C
          Serial.println("Raise Temp = 15");
          break;
      }
      switch (tempLowerString.toInt()) {
        case 25:
          tempToLower = 25;// 25 degrees C
          Serial.println("Lower Temp = 25");
          break;
        case 30:
          tempToLower = 30;// 30 degrees C
          Serial.println("Lower Temp = 30");
          break;
        case 35:
          tempToLower = 35;// 35 degrees C
          Serial.println("Lower Temp = 35");
          break;
        default:
          tempToLower = 20;// 20 degrees C
          Serial.println("Lower Temp = 20");
          break;

      }
    }
    if (responseArray[0].startsWith("GET /UP")) {
      raiseShade();
    }
    if (responseArray[0].startsWith("GET /DOWN")) {
      lowerShade();
    }

    printResponses();

  }
  // place code here if you want it to occur independent of user input from the website




}
