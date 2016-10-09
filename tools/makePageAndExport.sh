#!/bin/bash
python ./webpage2html/webpage2html.py -s http://localhost/EspLogger/index.html | gzip > ./../EspLogger/data/index.html.gz

echo "END"
