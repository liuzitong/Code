TEMPLATE = subdirs

SUBDIRS = client server

client.file = $$PWD/client/client.pro
server.file = $$PWD/server/server.pro

CONFIG += ordered
