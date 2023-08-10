TEMPLATE = subdirs

# load the sub project
SUBDIRS = svr cli

svr.file = $$PWD/svr/svr.pro
cli.file = $$PWD/cli/cli.pro

CONFIG += ordered

