import 'btlogic.dart';
import 'package:weather/weather.dart';

WeatherFactory wf = new WeatherFactory("6d61d31438b4e860362f8953862893c0");

Future<int> getWeather() async {
  try {
    //TODO: Fix this
    //Weather w = await wf.currentWeatherByLocation(lat, lon);
    Weather w = await wf.currentWeatherByCityName("Boulder");
    double fahrenheit = w.temperature.fahrenheit;
    return fahrenheit.toInt();

    return -10;
  } catch (e) {
    print(e);
  }
  return -69;
}
