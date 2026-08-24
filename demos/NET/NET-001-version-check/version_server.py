from flask import Flask

################
# pip install flask -i https://mirrors.aliyun.com/pypi/simple/
################

################
# 启动命令 python version_server.py
################


app = Flask(__name__)

LATEST_VERSION = "1.1.0"


@app.get("/version")
def version():
    return LATEST_VERSION


if __name__ == "__main__":
    app.run(
        host="127.0.0.1",
        port=8080
    )