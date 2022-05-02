import 'dart:ffi';

import 'package:flutter/material.dart';

import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:smartglasses/connectingPage.dart';
import 'package:smartglasses/global.dart';
import 'package:smartglasses/mainscreen.dart';
import 'package:smartglasses/notificationLogic.dart';
import 'package:smartglasses/weatherLogic.dart';
//import 'package:flutter_foreground_task/flutter_foreground_task.dart';
import 'dart:isolate';
import 'connection.dart';
import 'dart:convert';
import 'dart:typed_data';
import 'dart:async';

bool bluetoothCheck() {
  if (connection == null) return false;
}

void queueAdd(String data) {
  queue.add(data);
  if (queue.length > maxQueueLength) queue.removeAt(0);
}

void sendString(String data) {
  print("Attempting to send out: $data");
  //if (connection == null)
  //
  //else
  if (connection != null)
    try {
      connection.output.add(ascii.encode(data));
    } catch (e) {
      print("Error sending string...: ${e.toString()}");
      queueAdd(data);
      //startDiscovery();
    }
  else {
    print("CONNECTION IS LOST I THINK");
    queueAdd(data);
    //startDiscovery();
  }
}

void sendQueue() {
  while (queue.isNotEmpty) sendString(queue.removeAt(0));
}

void sync() async {
  int weather = await getWeather();
  DateTime now = DateTime.now();
  String data =
      '{"type":"sync","hours":"${now.hour.toString()}","minutes":"${now.minute.toString()}","seconds":"${now.second.toString()}","temperature":"${weather.toString()}"}"';
  sendString(data);
}

sendTeleprompterData(String data) {
  sendString('{"type":"teleprompter","data":"$data"}');
}

Future<String> startDiscovery() async {
  print("StartDiscovery fun");
  if (await FlutterBluetoothSerial.instance.isDiscovering) {
    print("we're already discovering, so don't.");
    return "bad";
  }
  //FlutterBluetoothSerial.instance.cancelDiscovery();

  String addr;
  var streamSubscription =
      FlutterBluetoothSerial.instance.startDiscovery().listen((r) {
    if (r.device.name == deviceName) {
      addr = r.device.address;
    }
  });
//TODO: figure out why this runs multiple times at once
  streamSubscription.onDone(() {
    print("BTDiscovery STREAM IS DONE!");
    FlutterBluetoothSerial.instance.cancelDiscovery();
    streamSubscription.cancel();
    if (addr == null) {
      print("Addr null...");
      startDiscovery();
    } else {
      print("Addr GOOD! Trying to connect.");
      testConnection(addr);
      return addr;
    }
  });
  streamSubscription.onError((e) {
    print("Error scanning: ${e.toString()}");
    testConnection(addr);
  });
  return "";
}

Future<bool> testConnection(String addr) async {
  try {
    connection = await BluetoothConnection.toAddress(addr);
    print('Connected to the device');
    print('connection:' + connection.toString());

    //sendString(notification1);
    sync();
    //sendQueue();
    connection.input.listen((Uint8List data) {
      print('Data incoming:');
      //Since TelescopeOS (v0.1) doesn't send any data, we can assume any data sent means it powered off :)

      try {
        print(ascii.decode(data));
      } catch (e) {
        print("Error decoding data. Data: ${data.toString()}");
      }
      //connection.output.add(data); // Sending data

      if (ascii.decode(data).contains('!')) {
        //connection.finish(); // Closing connection
        print('Disconnecting by local host');
      }
    }).onDone(() {
      print('Disconnected by remote request');
      //connection.finish();
      connection.dispose();
      connection = null;
      startDiscovery();
    });
  } catch (exception) {
    print('Cannot connect, exception occured');
    connection.dispose();
    connection = null;
    //connection.finish();
    startDiscovery();
  }

  return true;
}
