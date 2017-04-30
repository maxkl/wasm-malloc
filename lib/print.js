(function (Module) {
	'use strict';

	Module.libraries.push({
		name: 'print',
		import: function (imports) {
			var buffer = [];

			function printBuffered(str) {
				while(1) {
					var nl = str.indexOf('\n');
					if(nl !== -1) {
						console.log(buffer.join('') + str.substring(0, nl));
						buffer.length = 0;
						str = str.substring(nl + 1);
					} else {
						buffer.push(str);
						break;
					}
				}
			}

			function padLeft(padding, str) {
				return (padding + str).slice(-padding.length);
			}

			imports.printi = function (i) {
				printBuffered(i + '');
			};

			imports.printf = function (f) {
				printBuffered(f + '');
			};

			imports.printd = function (d) {
				printBuffered(d + '');
			};

			imports.printc = function (c) {
				printBuffered(String.fromCharCode(c));
			};

			imports.prints = function (ptr) {
				printBuffered(util.ptr2str(Module.memory.buffer, ptr));
			};

			imports.printptr = function (ptr) {
				if(ptr === 0) {
					printBuffered('(null)');
				} else {
					printBuffered('0x' + padLeft('00000000', ptr.toString(16)));
				}
			};
		}
	});

})(Module);
