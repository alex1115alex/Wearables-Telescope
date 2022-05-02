import 'dart:ffi';

import 'package:flutter/material.dart';

import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:smartglasses/global.dart';
import 'package:smartglasses/mainscreen.dart';
import 'package:smartglasses/notificationLogic.dart';
//import 'package:flutter_foreground_task/flutter_foreground_task.dart';
import 'dart:isolate';
import 'connection.dart';
import 'dart:convert';
import 'dart:typed_data';
import 'dart:async';
import 'btlogic.dart';

void main() {
  runApp(MyApp());
  //startDiscovery();
  //listenForNotifications();
  //btManager();
}

class MyApp extends StatelessWidget {
  // This widget is the root of your application.
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Flutter Demo',
      theme: ThemeData(
        primarySwatch: Colors.blue,
        visualDensity: VisualDensity.adaptivePlatformDensity,
      ),
      home: MainScreen(),
    );
  }
}
