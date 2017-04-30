(function (Module) {
	'use strict';

	Module.libraries.push({
		name: 'memory',
		import: function (imports) {
			imports.grow_memory = function (pages) {
				var oldPages = Module.memory.grow(pages);
				if(pages > 0) {
					console.log('Grew memory from ' + oldPages + ' to ' + (oldPages + pages) + ' pages');
				}
				return oldPages;
			};
		}
	});

})(Module);
