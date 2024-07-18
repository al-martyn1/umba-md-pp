@rem https://graphviz.org/doc/info/command.html
dot -Tsvg -s72 -o test001_72.svg test001.dot
dot -Tpng -s72 -o test001_72.png test001.dot
dot -Tsvg -Gdpi=300 -s96 -o test001_96.svg test001.dot
dot -Tpng -Gdpi=300 -s96 -o test001_96.png test001.dot

