import requests
import json


class MqttHttpClient:
    default_broker_url = "https://flespi.io/mqtt/messages"
    default_auth_token = "FlespiToken WtYmxCB4WNFnAMQCgKfAKgSxMOn8eLZY5N8qdzkZaOaAqO1xo6VZaBkMfxJh9w62"

    def __init__(self, broker_url=default_broker_url, auth_token=default_auth_token):
        self.brokerUrl = broker_url
        self.authToken = auth_token
        self.header_dict = {
            "Authorization": self.default_auth_token,
            "Content-Type": "application/json",
            "Accept": "application/json"
        }

    def publish(self, topic, message):
        if topic is None or message is None:
            raise Exception("Topic/Message cannot be None!")
        body = {"payload": message,
                "topic": topic,
                "type": 2}

        response = requests.post(url=self.brokerUrl, data=json.dumps(body), headers=self.header_dict)
        if response.status_code != 200:
            return 1
        return 0
