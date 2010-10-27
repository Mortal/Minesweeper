var Game = Base.extend({
//private vars:
	dimensions: null,
	state: 'INIT', // INIT, PLAY, WIN, LOSE
	minecount: 0,
	flagcount: 0,
	pressedcount: 0,
	tiles: null,
//public functions:
	constructor: function (dimensions) {
		this.dimensions = [];
		for (var i = 0; i < dimensions.length; ++i) {
			var d = parseInt(dimensions[i]);
			this.assert(d, 'Invalid dimension (idx '+i+')', 'constructor');
			this.dimensions.push(d);
		}
		this.state = 'INIT';
		this.minecount = 0;
		this.flagcount = 0;
		this.pressedcount = 0;
	},
	startgame: function (mines) {
		this.assert(this.state != 'PLAY', 'Game already in progress', 'startgame');
		this.inittiles(mines);
		this.state = 'PLAY';
	},
	neighbourhoodpositions: function (pos, includeself) {
		var r = [];
		this._neighbourhoodpositions(0, pos, !(!includeself), r);
		return r;
	},
	neighbourhood: function (pos, includeself) {
		var r = [];
		var positions = this.neighbourhoodpositions(pos, !(!includeself));
		for (var i = 0; i < positions.length; ++i) {
			r.push(this.getTile(positions[i]));
		}
	},
	getTile: function (pos) {
		var idx = this.coordstofieldindex(pos);
		return this.tiles[idx];
	},
	output: function (sb) {
	},
	amIDeadNow: function (pos) {
		this.assert(this.state == 'PLAY', 'Game not active', 'amIDeadNow');
		var tile = this.getTile(pos);
		if (!tile) return false;
		if (tile.getDepressed()) return false;
		this.press(pos);
		if (tile.amIDeadNow()) {
			this.state = 'LOSE';
			return true;
		}
		if (this.pressedcount == this.tiles.length-this.minecount) {
			this.state = 'WIN';
		}
		return false;
	},
	flagOn: function (pos) {
		var tile = this.getTile(pos);
		if (tile && tile.flagon()) {
			++this.flagcount;
		}
	},
	flagOff: function (pos) {
		var tile = this.getTile(pos);
		if (tile && tile.flagoff()) {
			--this.flagcount;
		}
	},
	getFlag: function (pos) {
		this.assert(this.tiles, '!this.tiles', 'getFlag');
		var tile = this.getTile(pos);
		return tile && tile.getFlag();
	},
	origo: function () {
		this.assert(this.dimensions, '!this.dimensions', 'origo');
		var pos = [];
		for (var i = this.getDimensionCount(); i--;) pos.push(0);
		return pos;
	},
	getDimensionCount: function () {this.assert(this.dimensions, '!this.dimensions', 'getDimensionCount'); return this.dimensions.length;},
	getState: function () {return this.state;},
	totalMines: function () {return this.minecount;},
	totalFlags: function () {return this.flagcount;},
//private functions:
	assert: function (p, s, f) {
		if (p) return;
		if (f) s = 'In member function '+f+': '+s;
		throw s;
	},
	inittiles: function (mines) {
		this.minecount = mines;
		this.tiles = [];
		this.pressedcount = 0;
		this.flagcount = 0;
		this.tilecount = 1;
		for (var i = 0; i < this.dimensions.length; ++i) {
			this.tilecount *= this.dimensions[i];
		}
		this.filltheblanks();
		this.deploymines();
		if (!this.pressblanks(0, this.origo())) this.pressrandom();
	},
	filltheblanks: function () {
		this.assert(this.tiles.length <= this.tilecount, 'Too many tiles wrt this.tilecount', 'filltheblanks');
		for (var i = 0; i < this.tiles.length; ++i) {
			if (!this.tiles[i]) this.tiles[i] = new Tile();
		}
		while (this.tiles.length < this.tilecount) {
			this.tiles.push(new Tile());
		}
	},
	deploymines: function () {
		var mines = this.minecount;
		if (!mines) return;
		var minelist = [];
		while (mines-- > 0 && this.tilecount > minelist.length) {
			var range = this.tilecount-this.minelist.length;
			var idx = Math.floor(range*Math.random());
			minelist.sort();
			for (var i = 0; i < minelist.length; ++i) {
				if (idx >= minelist[i]) ++idx;
			}
			this.assert(idx <= this.tilecount, 'Tile index out of bounds', 'deploymines');
			minelist.push(idx);
		}
		for (var i = 0; i < minelist.length; ++i) {
			this.tiles[minelist[i]].setBlarg(true);
			var neighbours = this.neighbourhood(this.fieldindextocoords(minelist[i]));
			for (var j = 0; j < neighbours.length; ++j) {
				j.addNeighbouringBomb();
			}
		}
	},
	pressblanks: function (dim, basis) {
		if (dim == this.getDimensionCount()) {
			var tile = this.getTile(basis);
			if (!tile.blarg && !tile.aroundtheworld) {
				this.amIDeadNow(basis);
				return true;
			}
			return false;
		} else {
			var r = false;
			for (var i = 0; i < this.dimensions[dim]; ++i) {
				var coords = tmg.cleanarray(basis);
				coords[dim] = i;
				if (this.pressblanks(dim+1, coords)) {
					r = true;
				}
			}
			return r;
		}
	},
	pressrandom: function () {
		var range = this.getTileCount()-this.totalMines();
		this.assert(range > 0, 'All tiles are mines', 'pressrandom');
		var idx = Math.floor(Math.random()*range);
		for (var i = 0, tileindex = 0; r && i < this.tiles.length; ++i, ++tileindex) {
			if (!this.tiles[i].blarg && !--r) this.amIDeadNow(this.fieldindextocoords(tileindex));
		}
	},
	_neighbourhoodpositions: function (dim, pos, includeself, result) {
		if (dim == this.getDimensionCount()) {
			if (includeself) result.push(pos);
		} else {
			if (pos[dim] > 0) {
				var prev = tmg.cleanarray(pos);
				--prev[dim];
				this._neighbourhoodpositions(dim+1, prev, true, result);
			}
			this._neighbourhoodpositions(dim+1, pos, includeself, result);
			if (pos[dim] < this.dimensions[dim]) {
				var next = tmg.cleanarray(pos);
				++next[dim];
				this._neighbourhoodpositions(dim+1, next, true, result);
			}
		}
	},
	coordstofieldindex: function (coords) {
		this.assert(coords.length == this.dimensions.length, 'Dimension count mismatch', 'coordstofieldindex');
		var idx = 0;
		for (var dim = 0; dim < this.dimensions.length; ++dim) {
			this.assert(coords[dim] < this.dimensions[dim], 'Out of bounds in dimension '+dim, 'coordstofieldindex');
			idx *= this.dimensions[dim];
			idx += coords[dim];
		}
		return idx;
	},
	fieldindextocoords: function (idx) {
		var result = this.origo();
		for (var dim = 0; dim < this.dimensions.length; ++dim) {
			var dim2 = dim;
			var coord = idx;
			for (++dim2; dim2 < this.dimensions.length; ++dim2) {
				coord = (coord - (coord % this.dimensions[dim2])) / this.dimensions[dim2];
			}
			coord %= this.dimensions[dim];
			result[dim] = coord;
		}
		return result;
	}
});
