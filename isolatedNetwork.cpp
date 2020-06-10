/******************isolatedServer() method********************/
/*
   This method is used to serve a website before the user's
   network parameters have been successfully loaded to the
   arduino's flash memory.

   The served web page provides the user with the ability
   to enter their network credentials for the arduino to
   store.

   The user must reset the arduino after providing the correct
   network credentials.
*/

void isolatedServer() {
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
          //print the network details that are stored in the flash memory. This should be blank information the 1st time through.
          printNetworkSettings();

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {


            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();


            // the remaing response provides the HTML that is used to render the webpage on the user's browser
            client.println("<!DOCTYPE html>"
"<html lang=\"en\" >"
"<head>");
// style sheet

client.println("<style>"
"@charset \"UTF-8\";"
"h1, p {"
"  color: #990033 !important;"
"  text-align: center;"
"  font-size: 250%;"
"}"
"body {"
"  position: absolute;"
"  left: 0;"
"  top: 0;"
"  width: 100%;"
"  height: 100%;"
"  font-family: \"calibri\", sans-serif;"
"  background-color: #feffba;"
"  overflow: hidden;"
"}"
".table {"
"  display: table;"
"  width: 100%;"
"  height: 100%;"
"}"
".table-cell {"
"  display: table-cell;"
"  vertical-align: middle;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container {"
"  position: relative;"
"  width: 600px;"
"  margin: 30px auto 0;"
"  height: 320px;"
"  background-color: #feff86;"
"  top: 50%;"
"  margin-top: -260px;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container .box {"
"  position: absolute;"
"  left: 0;"
"  top: 0;"
"  width: 100%;"
"  height: 100%;"
"  overflow: hidden;"
"}"
".container .box:before {"
"  content: \" \";"
"  position: absolute;"
"  left: 152px;"
"  top: 50px;"
"  background-color: #feffba;"
"  transform: rotateX(52deg) rotateY(15deg) rotateZ(-38deg);"
"  width: 300px;"
"  height: 285px;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container .box:after {"
"  content: \" \";"
"  position: absolute;"
"  left: 152px;"
"  top: 50px;"
"  background-color: #feff51;"
"  transform: rotateX(52deg) rotateY(15deg) rotateZ(-38deg);"
"  width: 300px;"
"  height: 285px;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"  background-color: #feffa7;"
"  top: -10px;"
"  left: 80px;"
"  width: 320px;"
"  height: 180px;"
"}"
".container .container-forms {"
"  position: relative;"
"}"
".container .container-forms .container-info {"
"  text-align: left;"
"  font-size: 0;"
"}");
client.println(".button {"
" background-color: Transparent; /* Green */"
 " border: none;"
 " color: white;"
 " cursor: pointer;"
          "}"
".container .container-forms .container-info .info-item {"
"  text-align: center;"
"  font-size: 16px;"
"  width: 300px;"
"  height: 320px;"
"  display: inline-block;"
"  vertical-align: top;"
"  color: #8C1D40;"
"  opacity: 1;"
"  -moz-transition: all 0.3s;"
"  -o-transition: all 0.3s;"
"  -webkit-transition: all 0.3s;"
"  transition: all 0.3s;"
"}"
".container .container-forms .container-info .info-item p {"
"  font-size: 20px;"
"  margin: 20px;"
"}"
".container .container-forms .container-info .info-item .btn {"
"  background-color: #A62708;"
"  border: 1.5px solid #B22222;"
"}"
".container .container-forms .container-info .info-item .table-cell {"
"  padding-right: 35px;"
"}"
".container .container-forms .container-info .info-item:nth-child(2) .table-cell {"
"  padding-left: 35px;"
"  padding-right: 0;"
"}"
".container .btn {"
"  cursor: pointer;"
"  text-align: center;"
"  margin: 0 auto;"
"  width: 60px;"
"  color: #fff;"
"  background-color: #990033;"
"  opacity: 1;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"  padding: 10px 15px;"
"}"
".container .btn:hover {"
"  opacity: 0.7;"
"}"
".container input {"
"  padding: 10px 15px;"
"  margin: 0 auto 15px;"
"  display: block;"
"  width: 220px;"
"  -moz-transition: all 0.3s;"
"  -o-transition: all 0.3s;"
"  -webkit-transition: all 0.3s;"
"  transition: all 0.3s;"
"}"
".container .container-form {"
"  overflow: hidden;"
"  position: absolute;"
"  left: 30px;"
"  top: -30px;"
"  width: 305px;"
"  height: 380px;"
"  background-color: #fff;"
"  box-shadow: 0 0 15px 0 rgba(0, 0, 0, 0.2);"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container .container-form:before {"
"  content: \"A\";"
"  position: absolute;"
"  left: 160px;"
"  top: -50px;"
"  color: #990033;"
"  font-size: 130px;"
"  opacity: 0;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container .container-form .btn {"
"  position: relative;"
"  box-shadow: 0 0 10px 1px #ff73b3;"
"  margin-top: 30px;"
"}"
".container .form-item {"
"  position: absolute;"
"  left: 0;"
"  top: 0;"
"  width: 100%;"
"  height: 100%;"
"  opacity: 1;"
"  -moz-transition: all 0.5s;"
"  -o-transition: all 0.5s;"
"  -webkit-transition: all 0.5s;"
"  transition: all 0.5s;"
"}"
".container .form-item.sign-up {"
"  position: absolute;"
"  left: -100%;"
"  opacity: 0;"
"}"
".container.log-in .box:before {"
"  position: absolute;"
"  left: 180px;"
"  top: 62px;"
"  height: 265px;"
"}"
".container.log-in .box:after {"
"  top: 22px;"
"  left: 192px;"
"  width: 324px;"
"  height: 220px;"
"}");
client.println(".container.log-in .container-form {"
"  left: 265px;"
"}");
client.println(".container.log-in .container-form .form-item.sign-up {"
"  left: 0;"
"  opacity: 1;"
"}"
".container.log-in .container-form .form-item.log-in {"
"  left: -100%;"
"  opacity: 0;"
"}"
".container.active {"
"  width: 260px;"
"  height: 140px;"
"  margin-top: -70px;"
"}"
".container.active .container-form {"
"  left: 30px;"
"  width: 200px;"
"  height: 200px;"
"}"
".container.active .container-form:before {"
"  content: \"B\";"
"  position: absolute;"
"  left: 51px;"
"  top: 5px;"
"  color: #990033;"
"  font-size: 130px;"
"  opacity: 1;"
"}"
".container.active input {"
"  display: none;"
"  opacity: 0;"
"  padding: 0px;"
"  margin: 0 auto;"
"  height: 0;"
"}"
".container.active .btn {"
"  display: none;"
"  opacity: 0;"
"  padding: 0px;"
"  margin: 0 auto;"
"  height: 0;"
"}"
".container.active .info-item {"
"  display: none;"
"  opacity: 0;"
"  padding: 0px;"
"  margin: 0 auto;"
"  height: 0;"
"}"
".container.active .form-item {"
"  height: 100%;"
"}"
".container.active .container-forms .container-info .info-item {"
"  height: 0%;"
"  opacity: 0;"
"}"
"ul {"
"  margin: 0.75em 3;"
"  padding: 4 3em;"
"  list-style: none;"
"}"
"li:before {"
"  content: \"\";"
"  border-color: transparent #990033;"
"  border-style: solid;"
"  border-width: 0.35em 0 0.35em 0.45em;"
"  display: block;"
"  height: 0;"
"  width: 0;"
"  left: -1em;"
"  top: 0.9em;"
"  position: relative;"
"}"
"</style>");

  client.println("<!DOCTYPE html>"
"<html lang=\"en\" >"
"<head>"
  "<meta charset=\"UTF-8\">"
  "<title>Log in Reset</title>"

"</head>"
"<body>"
"<!-- partial:index.partial.html -->"
"<br><br>"
"<h1> It's Always Sunny In Arizona</h1>"
"<div class=\"container\">"
  "<div class=\"box\"></div>"
  "<div class=\"container-forms\">"
    "<div class=\"container-info\">"
      "<div class=\"info-item\">"
        "<div class=\"table\">"
          "<div class=\"table-cell\">"
            "<p>"
              "Ready to log back in?"
            "</p>"
            "<div class=\"btn\">"
              "Log in"
            "</div>"
          "</div>"
        "</div>"
      "</div>"
      "<div class=\"info-item\">"
        "<div class=\"table\">"
          "<div class=\"table-cell\">"
            "<div class=\"btn\">"
              "<button class=\"button\" onclick=\"window.location.href = '/RESET';showPopup();\">Reset</button>"
            "</div>"
          "</div>"
        "</div>"
      "</div>"
    "</div>"
    "<div class=\"container-form\">"
      "<div class=\"form-item log-in\">"
        "<div class=\"table\">"
          "<div class=\"table-cell\">"
          "<form action=\"/\" method=\"GET\">"
            "<input name=\"SSID\" placeholder=\"Network SSID\" type=\"text\" /><input name=\"PWD\" placeholder=\"Network Password\" type=\"Password\" />"
            "<div class=\"btn\">"
            "<button class=\"button\" type=\"submit\">"  
              "Submit"
            "</button>"  
            "</div>"
          "</div>"
        "</div>"
      "</div>"
      "<div class=\"form-item sign-up\">"
 "<ul><br><br><br>"
  "<li>Allow 60 seconds for device to reboot</li>"
  "<li>Sign into your network</li>"
  "<li>Point your browser to http://asacontroller.</li>"
"</ul>"
          "</div>"
        "</div>"
      "</div>"
    "</div>"
  "</div>"
"</div>"
"<!-- partial -->");
// javascript

 client.println("<script>"
"function showPopup()"
      "{ alert(\"Follow these steps:\\n1) Close this Popup \\n2) Allow 60 seconds for your device to reboot\\n3) Join your home network \\n4) Point your browser to http://asacontroller.\")};"
                           
   "</script>"); 

client.println("</body></html>");
//"$(\".container\").addClass(\"active\");"



/*
 * OLD WEBSITE
 * "<html>"
                           "<head>"
                           "<style>"
                           "body {  background-color: #8C1D40;}"
                           "h1 { color: #FFC627; text-align: center;}"
                           "p {  color: #FFC627; text-align: center; font-weight: bold; font-family: verdana;  font-size: 20px;}"
                           "label { font-family: verdana; font-size: 20px; padding: 20px;}"
                           "div {margin: 10px;}"
                           ".config {"
                           "margin: auto;"
                           "padding: 10px;"
                           "}"
                           "</style>"
                           "</head>"
                           "<body>"
                           "<div align=\"center\">"
                           "<canvas id=\"myCanvas\" width=\"600\" height=\"75\"></canvas>"
                           "</div>"
                           "<tr><td colspan=\"2\" align=\"center\"><hr/></td>"        
                           "<tr><td colspan=\"2\" align=\"center\">");
                           client.print(printReport());
                           client.println("</td></tr>"
                           "<script>"
                           "function showPopup()"
                           "{ alert(\"Follow these steps:\\n1) Close this Popup \\n2) Allow 60 seconds for your device to reboot\\n3) Join your home network \\n4) Point your browser to http://asacontroller.\")};"
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
                           "</script>"
                           "<form action=\"/\" method=\"GET\">"
                           "<div class=\"config\">"
                           "<table align=\"center\">"
                           "<tr>"
                           "<td><label for=\"SSID\">Network SSID</label></td>"
                           "<td><input name=\"SSID\" id=\"SSID\" value=\"\"></td>"
                           "</tr>"
                           "<tr>"
                           "<td><label for=\"PWD\">Network Password</label></td>"
                           "<td><input name=\"PWD\" id=\"PWD\" value=\"\"></td>"
                           "</tr>"
                           "<tr>"
                           "<td style=\"text-align:center;\" colspan=\"2\"><button>Submit</button></td>"
                           "</tr>"
                           "</table>"
                           "</div>"
                           "</form>"
                           "<p>Please enter your network SSID and password to setup your shade controller</p>"
                           "<p>"
                           "<button type=\"button\" onclick=\"window.location.href = '/RESET';showPopup();\">Reset</button>"
                           "</p>"
                           "</body>"
                           "</html>"
 */
            // The HTTP response ends with another blank line:
            client.println();
            // break out of the while loop:
            break;
          }
          // if the current line is not blank then there is additional processing before rendering the page.
          // we will store upto 10 lines of data from the user's request as items in the responseArray
          else {
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
      }
    }
    // close the connection:
    client.stop();
    Serial.println("client disconnected");

    /* This section of code is used to store the SSID and Password.  logic:
        if the response from the client has a line that starts with Get /?SSID and the network is not already valid then...
        1. locate the value of the SSID in the response string and save it to the flash memory
        2. locate the value of the password in the response string and save it to the flash memory
    */
    if (responseArray[0].startsWith("GET /?SSID") && String(thisNetwork.valid) != "valid") {
      String thisString = responseArray[0];
      String netValid = "valid";
      int ssidStartIndex = thisString.indexOf("?") + 6;
      int ssidEndIndex = thisString.indexOf("&");
      int passwordStartIndex = thisString.indexOf("&") + 5;
      int passwordEndIndex = thisString.length() - 9;
      String netID = thisString.substring(ssidStartIndex, ssidEndIndex);
      Serial.println(netID);
      netID.toCharArray(thisNetwork.ssid, 100);
      String netPass = thisString.substring(passwordStartIndex, passwordEndIndex);
      Serial.println(netPass);
      netPass.toCharArray(thisNetwork.password, 100);
      netValid.toCharArray(thisNetwork.valid, 100);
      printLine();
      Serial.println("writing network settings");
      storedNetwork.write(thisNetwork);
      Serial.println("done");
      printLine();
    }

    // Check to see if the client request was to reset the hardware
    if (responseArray[0].startsWith("GET /RESET")) {
          MyWatchDog.setup(WDT_HARDCYCLE250m);
        }
    printResponses();

  }
}
