bool socket_init(String server,int port,String url)
{
  client.onMessage(onMessageCallback);
  client.onEvent(onEventsCallback);

  return client.connect(server,port,url);
}

void onMessageCallback(WebsocketsMessage message) {
    Serial.print("Got Message: ");
    Serial.println(message.data());
    /***
     * Get time rendered from the time it on
     */
    JsonObject& root = jsonBuffer.parseObject(message.data());
    if(root["get_time"])
    {
      timeRendered = root["timeRendered"];
      Serial.print("Time used => ");
      Serial.println(timeRendered);
    }
}

void onEventsCallback(WebsocketsEvent event, String data) {
    if(event == WebsocketsEvent::ConnectionOpened) {
        Serial.println("Connnection Opened");
    } else if(event == WebsocketsEvent::ConnectionClosed) {
        Serial.println("Connnection Closed");
    } else if(event == WebsocketsEvent::GotPing) {
        Serial.println("Got a Ping!");
    } else if(event == WebsocketsEvent::GotPong) {
        Serial.println("Got a Pong!");
    }
}
