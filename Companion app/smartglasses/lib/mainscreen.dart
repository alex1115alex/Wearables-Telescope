import 'package:flutter/material.dart';
import 'package:smartglasses/btlogic.dart';
import 'package:smartglasses/drinkIdea.dart';
import 'package:smartglasses/global.dart';
import 'dart:convert';

import 'package:smartglasses/notificationLogic.dart';

class MainScreen extends StatefulWidget {
  @override
  State<MainScreen> createState() => MainScreenState();
}

class MainScreenState extends State<MainScreen> {
  @override
  void initState() {
    super.initState();
    startDiscovery();
    listenForNotifications();
  }

  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(title: Text("Telescope Companion")),
        body: Container(
          padding: EdgeInsets.all(20),
          child: connection != null
              ? Container(
                  child: Text("CONNECTING...", style: TextStyle(fontSize: 34)))
              : Column(
                  children: [
                    Text("Connected",
                        style: TextStyle(fontSize: 24, color: Colors.green)),
                    RaisedButton(
                        child: Text("Simulate Button Press"),
                        onPressed: sendDummyNotification),
                    SizedBox(
                      height: 20,
                    ),
                    RaisedButton(
                        child: Text("Manually sync time and weather"),
                        onPressed: sync),
                    SizedBox(
                      height: 20,
                    ),
                    RaisedButton(
                        child: Text("Send test notification"),
                        onPressed: sendNotif),
                    SizedBox(height: 20),
                    RaisedButton(
                        child: Text("Manually verify connection to console"),
                        onPressed: checkConnection),
                    SizedBox(height: 20),
                    RaisedButton(
                        child: Text("Send light theme"),
                        onPressed: () => sendTheme("light")),
                    SizedBox(height: 20),
                    RaisedButton(
                        child: Text("Send dark theme"),
                        onPressed: () => sendTheme("dark")),
                    SizedBox(height: 40),
                    RaisedButton(
                        child: Text("GET NEW FAKE DRINK"),
                        onPressed: () =>
                            sendTeleprompterData(getFakeDrinkName())),
                    SizedBox(height: 20),
                  ],
                ),
        ));
  }

  sendNotif() {
    String notification1 =
        '{"type":"notif","title":"TestAppTitle","content":"testappcontent"}';
    sendString(notification1);
  }

  sendDummyNotification() {
    sendString('{"type":"next"}');
  }

  checkConnection() {
    print(connection.toString());
  }

  // I lost my odbII reader :(
  sendCarData(int speed, int rpm) {
    sendString(
        '{"type":"car","speed":"${speed.toString()}","rpm":"${rpm.toString()}"}');
  }

  sendTheme(String theme) {
    sendString('{"type":"theme","id":"$theme"}');
  }
}
