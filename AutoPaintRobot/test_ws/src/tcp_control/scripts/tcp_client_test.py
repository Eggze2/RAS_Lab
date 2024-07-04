import socket
import time
import struct
import threading

def create_message(counter):
    # 固定前缀部分
    prefix = bytes([0x08, 0x00, 0x00, 0x06, 0x01])
    # 计数器部分，由8字节的大端格式整数表示
    counter_bytes = struct.pack('!Q', counter)  # 8字节大端整数
    return prefix + counter_bytes

def receive_messages(client_socket):
    while True:
        try:
            data = client_socket.recv(1024)
            if data:
                print(f"Received from server: {data.hex()}")
            else:
                print("Server closed connection")
                break
        except socket.error as e:
            print(f"Socket error: {e}")
            break

def send_messages(client_socket):
    counter = 0
    start_time = time.time()
    
    while True:
        current_time = time.time()
        if current_time - start_time >= 1:
            message = create_message(counter)
            try:
                client_socket.sendall(message)
                print(f"Sent to server: {message.hex()}")
            except socket.error as e:
                print(f"Socket error: {e}")
                break
            counter += 1
            start_time = current_time

def main():
    # 设置TCP连接的服务器地址和端口
    server_ip = '192.168.0.10'  # 替换为实际的服务器IP
    server_port = 7000      # 替换为实际的服务器端口

    # 创建TCP套接字
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    
    try:
        # 连接到服务器
        client_socket.connect((server_ip, server_port))
        print(f'Connected to {server_ip}:{server_port}')
        
        # 启动接收消息的线程
        receive_thread = threading.Thread(target=receive_messages, args=(client_socket,))
        receive_thread.daemon = True
        receive_thread.start()
        
        # 启动发送消息的线程
        send_thread = threading.Thread(target=send_messages, args=(client_socket,))
        send_thread.daemon = True
        send_thread.start()
        
        # 主线程保持运行，直到接收线程或发送线程出错
        receive_thread.join()
        send_thread.join()

    except Exception as e:
        print(f"Exception: {e}")
    finally:
        # 关闭套接字
        client_socket.close()
        print("Connection closed")

if __name__ == "__main__":
    main()
