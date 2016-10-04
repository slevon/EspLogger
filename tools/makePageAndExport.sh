#!/bin/bash
cp -p /var/www/html/page .
python ./webpage2html/webpage2html.py -s http://localhost/page/index.html | gzip > ./../EspLogger/data/index.html.gz

echo "END"
