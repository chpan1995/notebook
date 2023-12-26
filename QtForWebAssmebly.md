# &#x20;编译成html改名为index.htm

&#x9;1\. 这样用Python服务起动是自动找index.html不需要加网页后缀 (python3 -m http.server)

&#x9;2\. 用singleThread编译的话则不会出现 Application exit (SharedArrayBuffer is not defined)错误

&#x20;  3\. 用Python和Nginx部署都可以

# 用multiThread编译需https才能访问

配置 Nginx 的 HTTPS 访问主要涉及到生成 SSL 证书（自签名或购买）并配置 Nginx 以正确使用这些证书。以下是一个基本步骤：

### 1. 生成 SSL 证书

\
如果你没有已经准备好的 SSL 证书，你可以生成一个自签名证书。可以使用 OpenSSL 来创建：\
shell

```
openssl req -new -x509 -days 365 -nodes -out cert.pem -keyout key.pem

```

\
这将在当前目录生成 `cert.pem` 和 `key.pem` 两个文件。这些就是你的 SSL 证书和私钥，你将在下一步配置 Nginx 时使用。

### 2. 配置 Nginx

\
在你的 Nginx 配置文件（通常是 `/etc/nginx/nginx.conf` 或者 `/etc/nginx/sites-available/default`） 中，你需要增加一个新的 server 块来处理 HTTPS 请求。以下是一个基本的配置示例：\
nginx

```
server {
    listen 443 ssl;
    server_name your_domain_or_IP;

    ssl_certificate /etc/nginx/ssl/cert.pem;
    ssl_certificate_key /etc/nginx/ssl/key.pem;

    location / {
        root /usr/share/nginx/html;
        index index.html index.htm;
    }
}

```

\
将 `your_domain_or_IP` 替换为你的域名或者服务器 IP 地址，将 `/etc/nginx/ssl/cert.pem` 和 `/etc/nginx/ssl/key.pem` 替换为你的证书和私钥的实际路径。\
如果你希望将 HTTP 流量自动重定向到 HTTPS，你可以增加一个额外的 server 块：\
nginx

```
server {
    listen 80;
    server_name your_domain_or_IP;
    return 301 https://$host$request_uri;
}

```

### 3. 重启 Nginx

\
保存并退出 Nginx 配置文件，然后通过命令重启 Nginx 以使新的配置生效。\
shell

```
sudo systemctl restart nginx

```

\
现在，你可以通过浏览器在 HTTPS 端口访问你的网站了，如 `https://your_domain_or_IP`。如果你的证书是自签名的，浏览器可能会警告你，你需要手动接受此警告才能访问你的网站。

# &#x20;python 部署生成https访问

\
以下是一个使用 Python 的 `http.server` 模块创建 HTTPS 服务器的示例：\
首先，你需要创建一个自签名的 SSL 证书。你可以使用 OpenSSL 来创建。在命令行中运行以下命令：\
shell

```
openssl req -new -x509 -keyout server.pem -out server.pem -days 365 -nodes

```

\
然后，在你的网站的根目录创建一个名为 `server.py` 的文件，内容如下：\
python

```
import http.server, ssl
server_address = ('localhost', 8000)
httpd = http.server.HTTPServer(server_address, http.server.SimpleHTTPRequestHandler)
httpd.socket = ssl.wrap_socket(httpd.socket,
                               server_side=True,
                               certfile='server.pem', 
                               ssl_version=ssl.PROTOCOL_TLS)
httpd.serve_forever()

```

\
然后运行这个 `server.py` 文件：\
shell

```
python3 server.py

```

\
这样，你就可以通过 `https://localhost:8000` 在浏览器中访问你的网站了。\
请注意，因为这个 SSL 证书是自签名的，所以浏览器会警告你连接不安全。你需要手动接受这个警告才能访问网站。此外，这个示例中我们使用的是本地主机（localhost），如果你想在其他的设备上访问，你需要将 'localhost' 替换为你服务器的 IP 地址，并确保防火墙设置允许这个 IP 和端口的连接。





