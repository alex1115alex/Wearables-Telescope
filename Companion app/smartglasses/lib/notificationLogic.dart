import 'dart:ffi';

import 'package:flutter/material.dart';

import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:smartglasses/btlogic.dart';
import 'package:smartglasses/global.dart';
import 'package:smartglasses/mainscreen.dart';
//import 'package:flutter_foreground_task/flutter_foreground_task.dart';
import 'dart:isolate';
import 'connection.dart';
import 'dart:convert';
import 'dart:typed_data';
import 'package:notifications/notifications.dart';
import 'dart:async';
import 'package:intl/intl.dart';

Notifications _notifications;
StreamSubscription<NotificationEvent> _subscription;

List<String> packageBlacklist = [
  "com.android.systemui",
  "com.samsung.android.app.smartcapture"
];

String sanitize(String str) {
  String characterFilter = "[^\\p{L}\\p{M}\\p{N}\\p{P}\\p{Z}\\p{Cf}\\p{Cs}\\s]";
  return str.replaceAll(characterFilter, "");
}

void onData(NotificationEvent event) {
  if (!packageBlacklist.contains(event.packageName)) {
    print("Got a notification;");
    print(sanitize(event.toString()));

    String timeString = DateFormat('jm').format(event.timeStamp);

    String title = event.title;
    if (event.packageName == "com.google.android.gm")
      title = "GMail - " + event.title;

    String notification =
        '{"type":"notif","title":"$title","content":"${event.message}","time":"$timeString"}';

    sendString(notification);
  }
}

void listenForNotifications() {
  _notifications = new Notifications();
  try {
    _subscription = _notifications.notificationStream.listen(onData);

    _subscription.onDone(
      () {
        listenForNotifications();
      },
    );
    print('listening for notifications..');
  } on NotificationException catch (exception) {
    print(exception);
  }
}
