import 'package:flutter/material.dart';
import 'package:smartglasses/btlogic.dart';
import 'package:smartglasses/global.dart';
import 'dart:convert';

class ConnectingScreen extends StatefulWidget {
  @override
  State<ConnectingScreen> createState() => ConnectingScreenState();
}

class ConnectingScreenState extends State<ConnectingScreen> {
  @override
  Widget build(BuildContext context) {
    return Scaffold(
        appBar: AppBar(title: Text("Telescope Companion")),
        body: Container(
            padding: EdgeInsets.all(20),
            child: Container(
                child: Text("CONNECTING...",
                    style: TextStyle(fontSize: 34, color: Colors.red)))));
  }
}
