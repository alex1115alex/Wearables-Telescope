import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';

const int maxQueueLength = 10;

String hardcodedESP32BluetoothAddress = "AC:0B:FB:6F:6D:C2";
BluetoothConnection connection;
String deviceName = "ESP32test";

List<String> queue = [];
