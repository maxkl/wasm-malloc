(function (Module) {
	'use strict';

	function main() {
		var imports = {};
		imports.env = {};

		for (var i = 0; i < Module.libraries.length; i++) {
			var lib = Module.libraries[i];
			if (lib.import) {
				lib.import(imports.env);
			}
		}

		return WebAssembly.instantiateStreaming(fetch('build/main.wasm'), imports)
			.then(function (result) {
				var instance = result.instance;

				if (!instance.exports.memory) {
					throw new Error('Module does not export its linear memory');
				}

				Module.memory = instance.exports.memory;

				for (var i = 0; i < Module.libraries.length; i++) {
					var lib = Module.libraries[i];
					if (lib.init) {
						lib.init(instance);
					}
				}

				window.instance = instance;
				console.log('Module instantiated');
			});
	}

	main().catch(function (err) { console.error(err); });

})(Module);
