from http.server import BaseHTTPRequestHandler, ThreadingHTTPServer
from threading import Event

release = Event()

svg = b'''<svg xmlns="http://www.w3.org/2000/svg" width="200" height="200">
    <rect width="200" height="200" fill="red"/>
</svg>'''

class Handler(BaseHTTPRequestHandler):
    def do_GET(self):
        if self.path == "/release":
            release.set()
            self.send_response(200)
            self.end_headers()
            self.wfile.write(b"Released")
            return

        if self.path == "/svg":
            print("SVG request received. Waiting...", flush=True)

            # 不返回响应，让客户端协程持续挂起。
            release.wait()

            self.send_response(200)
            self.send_header("Content-Type", "image/svg+xml")
            self.send_header("Content-Length", str(len(svg)))
            self.send_header("Cache-Control", "no-store")
            self.end_headers()
            self.wfile.write(svg)
            return

        self.send_error(404)

ThreadingHTTPServer(("127.0.0.1", 8765), Handler).serve_forever()