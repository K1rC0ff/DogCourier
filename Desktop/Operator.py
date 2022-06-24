import PySimpleGUI as sg
import paho.mqtt.client as mqtt
import time
from array import *

container1 = container2 = container3 = container4 = container5 = "0"
container6 = container7 = container8 = op = "0"
containers = ['0', '0', '0', '0', '0', '0', '0', '0', '0']
payloads = ['0', '0', '0', '0', '0', '0', '0', '0', '0']

sg.theme('LightBlue')
layout = [
    [sg.Text('Оператор'), sg.InputText(key='OPERATOR')],
    [sg.Text('Ячейка 1 '), sg.InputText(key='CONTAINER 1'), sg.Text('Пусто', key='CONT1_STATUS')],
    [sg.Text('Ячейка 2 '), sg.InputText(key='CONTAINER 2'), sg.Text('Пусто', key='CONT2_STATUS')],
    [sg.Text('Ячейка 3 '), sg.InputText(key='CONTAINER 3'), sg.Text('Пусто', key='CONT3_STATUS')],
    [sg.Text('Ячейка 4 '), sg.InputText(key='CONTAINER 4'), sg.Text('Пусто', key='CONT4_STATUS')],
    [sg.Text('Ячейка 5 '), sg.InputText(key='CONTAINER 5'), sg.Text('Пусто', key='CONT5_STATUS')],
    [sg.Text('Ячейка 6 '), sg.InputText(key='CONTAINER 6'), sg.Text('Пусто', key='CONT6_STATUS')],
    [sg.Text('Ячейка 7 '), sg.InputText(key='CONTAINER 7'), sg.Text('Пусто', key='CONT7_STATUS')],
    [sg.Text('Ячейка 8 '), sg.InputText(key='CONTAINER 8'), sg.Text('Пусто', key='CONT8_STATUS')],
    [sg.Text('Статус: '), sg.Text('Нажмите "Обновить"', key='STATUS')],
    [sg.Button('ОК', button_color='lime'), sg.Button('Обновить'), sg.Button('Выход', button_color='red')]
]

window = sg.Window('Собака-курьер', layout)

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    for i in range(8):
        client.subscribe("to/client/cont"+str(i+1))
    client.subscribe("to/client/op")

def on_message(client, userdata, msg):
    global containers, payloads

    print(msg.topic+" "+msg.payload.decode())

    if msg.topic == "to/client/op":
        containers[0] = msg.payload.decode()
    for i in range(8):
        if msg.topic == ("to/client/cont"+str(i+1)):
            payloads[i+1] = msg.payload.decode()[0]
            if payloads[i+1] != '2':
                containers[i+1] = msg.payload.decode()[2:len(msg.payload.decode())]


client = mqtt.Client("CourierTest")
client.on_connect = on_connect
client.on_message = on_message
client.connect("dev.rightech.io", 1883, 60)


client.loop_start()
while True:
    event, values = window.read()

    if event == 'Обновить':
        client.on_connect = on_connect
        client.on_message = on_message
        client.connect("dev.rightech.io", 1883, 60)

        print(containers)
        print(payloads)

        window['OPERATOR'].update(containers[0])
        for i in range(8):
            window['CONTAINER '+str(i+1)].update(containers[i+1])
            if payloads[i+1] == '0':
                window['CONT'+str(i+1)+'_STATUS'].update('Пусто', text_color='black')
            elif payloads[i+1] == '1':
                window['CONT'+str(i+1)+'_STATUS'].update('Заполнено', text_color='orange')
            elif payloads[i+1] == '2':
                window['CONT'+str(i+1)+'_STATUS'].update('Изъято', text_color='green')
            else:
                window['CONT'+str(i+1)+'_STATUS'].update('Ошибка', text_color='red')

        window['STATUS'].update('Данные загружены', text_color = 'black')

    if event == 'ОК':
        if values['OPERATOR'] != "" or values['CONTAINER 1'] != "" or values['CONTAINER 2'] != "" or values['CONTAINER 3'] != "" or values['CONTAINER 4'] != "" or values['CONTAINER 5'] != "" or values['CONTAINER 6'] != "" or values['CONTAINER 7'] != "" or values['CONTAINER 8'] != "":
            if values['OPERATOR'] != "":
                client.publish("to/client/op", values['OPERATOR'], retain=True, qos=1)
            for i in range(8):
                if values['CONTAINER '+str(i+1)] != "":
                    if values['CONTAINER '+str(i+1)] == '0':
                        client.publish("to/client/cont"+str(i+1), '0|0', retain=True, qos=1)
                        window['CONT'+str(i+1)+'_STATUS'].update('Пусто', text_color='black')
                    else:
                        client.publish("to/client/cont"+str(i+1), '1|'+values['CONTAINER '+str(i+1)], retain=True, qos=1)
                        window['CONT'+str(i+1)+'_STATUS'].update('Заполнено', text_color='orange')
                    containers[i+1] = values['CONTAINER '+str(i+1)]

            window['STATUS'].update('Отправлено', text_color='lime')
        else:
            window['STATUS'].update('Заполните хотя бы одну ячейку', text_color='red')

    if event == ('Выход') or event == sg.WINDOW_CLOSED:
        break
client.loop_stop()
window.close()
