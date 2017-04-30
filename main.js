(function (Module) {
	'use strict';

	function fetchAndCompile(url) {
		return fetch(url)
			.then(function (response) { return response.arrayBuffer(); })
			.then(function (buffer) { return WebAssembly.compile(buffer); });
	}

	function main() {
		return fetchAndCompile('build/main.wasm').then(function (module) {
			var imports = {};
			imports.env = {};

			for(var i = 0; i < Module.libraries.length; i++) {
				var lib = Module.libraries[i];
				if(lib.import) {
					lib.import(imports.env);;
				}
			}

			var instance = new WebAssembly.Instance(module, imports);

			if(!instance.exports.memory) {
				throw new Error('Module does not export its linear memory');
			}

			Module.memory = instance.exports.memory;

			for(var i = 0; i < Module.libraries.length; i++) {
				var lib = Module.libraries[i];
				if(lib.init) {
					lib.init(instance);
				}
			}

			window.instance = instance;
			console.log('Module instantiated');
		});
	}

	main().catch(function (err) { console.error(err); });

})(Module);
