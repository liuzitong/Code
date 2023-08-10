TEMPLATE = subdirs

# load the sub project
SUBDIRS = stdio_reader stdio_caller

stdio_caller.file = $$PWD/caller/stdio_caller.pro
stdio_reader.file = $$PWD/reader/stdio_reader.pro

CONFIG += ordered

