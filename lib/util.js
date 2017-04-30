(function (Module) {
	'use strict';

	Module.libraries.push({
		name: 'util',
		import: function (imports) {
			imports.random = Math.random;
		}
	});

})(Module);
