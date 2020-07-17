import json 


def get_json_game(path):
    file = open(path, "r")
    json_ = json.loads(file.read())
    return json_


