#include <WiFi.h>
#include <WebServer.h>
#include <HTTPClient.h>
#include "auth.h"

String PostRequest(String url, float temperature, float humidity)
{
  HTTPClient http;
  String data = "?temperature=" + String(temperature) + "&humidity=" + String(humidity);
  http.begin(url + data); // beginは必要、データを投げるだけならクエリでこれだけで完了出来る
  //リダイレクトして、データを送る
  const char *headerNames[] = {"Location"}; // Locationをとる
  http.collectHeaders(headerNames, sizeof(headerNames) / sizeof(headerNames[0]));

  Serial.print("[HTTP POST] ...\n");
  int httpCode = http.POST(data);
  int status_code = http.GET();
  Serial.printf("\npost request: status code = %d\n", httpCode);
  Serial.printf("\nget request: status code = %d\n", status_code);

  if (status_code == HTTP_CODE_FOUND)
  {
    //リダイレクトされた 302
    String payload = http.getString();
    Serial.println(payload);

    // ヘッダのLocation（リダイレクト先URL）を取り出す
    Serial.println("Location");
    Serial.println(http.header("Location"));

    // リダイレクト先にGetリクエスト
    String redirect_url = http.header("Location");
    http.begin(redirect_url); // HTTP
    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
      String res = http.getString(); //目的のレスポンスが得られる
      Serial.println("GET RESPONSE!");
      Serial.println(res);
      return res;
    }
    else
    {
      Serial.print("[HTTP POST] GET... failed, error:");
      Serial.println(http.errorToString(httpCode).c_str());
    }
    http.end();
  }
  else
  {
    Serial.print("Not found!");
  }
  return "-1";
}

void setup()
{
  // WiFiのアクセスポイントに接続
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
  // ESP32のIPアドレスを出力
  Serial.println("WiFi Connected.");
  Serial.print("IP = ");
  Serial.println(WiFi.localIP());
  pinMode(15, OUTPUT);
  Serial.begin(115200);
  //データを投げる
  String res = PostRequest(host, 12, 55);
}
int n = 0;
void loop()
{
  //起動確認のみ
  digitalWrite(15, HIGH);
  delay(500);
  digitalWrite(15, LOW);
  delay(500);
  Serial.println(n++);
}