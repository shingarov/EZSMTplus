
external-tools: gringo z3 cvc4

gringo: /usr/local/bin/gringo-4.5.3
	ln -s /usr/local/bin/gringo-4.5.3 gringo
	ln -s /usr/local/bin/clingo-4.5.3 clingo
	ln -s /usr/local/bin/reify-4.5.3  reify

/usr/local/bin/gringo-4.5.3:
	echo "Gringo binary v4.5.3 not found in /usr/local/bin."
	echo "Please extract 'gringo' (md5: 5043070983ccbca2e989a5ba8865ee93)"
	echo "from gringo-4.5.3-linux-x86_64.tar.gz or build one from source,"
	echo "and install it as /usr/local/bin/gringo-4.5.3"
	false

cvc4: /usr/bin/cvc4
	ln -s /usr/bin/cvc4 cvc4

z3: /usr/bin/z3
	ln -s /usr/bin/z3 z3
