#!/usr/bin/env node
var PNG = require('pngjs').PNG;
var fs = require("fs");

var options = {
    inputImages: [],
    outputTilesName: 'tiles',
    outputPalettesName: 'palettes',
    outputFileName: 'Resources.h',
    outputConstPrefix: 'UCPPU_',
};

var args = [].slice.call(process.argv, 2);

var flaggable = {
    '-t': function (arg) {
        options.outputTilesName = arg;
    },
    '-p': function (arg) {
        options.outputPalettesName = arg;
    },
    '-h': function (arg) {
        options.outputFileName = arg;
    },
    '-c': function (arg) {
        options.constPrefix = arg;
    }
}

while (args.length) {
    var value = args.shift();
    if (flaggable[value]) {
        flaggable[value](args.shift());
        continue;
    }
    options.inputImages.push(value);
}

if (args.length) {
    usage("Too many arguments");
}

if (!options.inputImages.length) {
    usage("Please specify a PNG image file!");
}

function usage(message) {
    var out = [];
    if (message) {
        out.push(message);
    }
    out.push("Usage: " + process.argv[1].replace(/^.*\//, '') + " [-t ...] [-p ...] [-h ...] [-c ...] <inputPNGList>");
    out.push("     -t tileTableName     Name used for the `tiles` table");
    out.push("     -p paletteTableName  Name used for the `palettes` table");
    out.push("     -h headerFileName    Name of the header file ('include `.h`, please)");
    out.push("     -c constantPrefix    Prefix for the length constants (e.g., UCPPU_)");
    throw new Error(out.join('\n'));
}

function Palette() {
    Object.defineProperty(this, 'length', {
        enumerable: false,
        writable: false,
        configurable: true,
        value: 1
    });
    this.hash = {};
}
function hex(n, l) {
    n = n.toString(16); 
    while (n.length < l) { n = '0' + n; }
    return n;
}
Palette.prototype.add = function (r, g, b, a) {
    if (a != 255) { return; }
    var rgb565 = hex((((r * 31 / 255) << 11) | ((g * 63 / 255) << 5) | (b * 31 / 255)), 4);
    
    if (!this.hash[rgb565]) {
        
        if (this.length === 16) {
            throw new Error("Too many colors!");
        }
        
        this.hash[rgb565.toString(16)] = true;
        
        Object.defineProperty(this, 'length', {
            enumerable: false,
            writable: false,
            configurable: true,
            value: this.length + 1
        });

    }
    return;
};

Palette.prototype.lookup = function (r, g, b, a) {
    if (a != 255) { return 0; }
    var rgb565 = hex((((r * 31 / 255) << 11) | ((g * 63 / 255) << 5) | (b * 31 / 255)), 4);
    if (this.hash[rgb565]) {
        return this.hash[rgb565];
    }
    return null;
};

Palette.prototype.sort = function () {
    var colors = Object.keys(this.hash).sort();
    var list = [ 'TRANSPARENT' ].concat(colors);
    this.hash = {};
    list.forEach(function (color, index) {
        if (color === 'TRANSPARENT') { return; }
        this.hash[color] = index;
    }, this);
    return this;
};

Palette.prototype.toC = function () {
    var list = ['0x0000'].concat(Object.keys(this.hash).map((item) => '0x' + item));
    while (list.length < 16) {
        list.push('0x0000');
    }
    return '{ ' + list.join(', ') + ' }';
};

function Tile() {
    this.data = new Array(64);
    for (var i = 0; i < this.data.length; i++) {
        this.data[i] = 0;
    }
}
Tile.prototype.setPixel = function (x, y, c) {
    this.data[y * 8 + x] = c;
}
Tile.prototype.toC = function () {
    var out = [];
    for (var y = 0; y < 8; y++) {
        out.push('0x' + 
            (this.data[y*8+0].toString(16) || '0')+ 
            (this.data[y*8+1].toString(16) || '0')+ 
            (this.data[y*8+2].toString(16) || '0')+ 
            (this.data[y*8+3].toString(16) || '0')+ 
            (this.data[y*8+4].toString(16) || '0')+ 
            (this.data[y*8+5].toString(16) || '0')+ 
            (this.data[y*8+6].toString(16) || '0')+ 
            (this.data[y*8+7].toString(16) || '0')
        );
    }
    return '{\n  ' + out.join(',\n  ') + '\n}';
};

var palettes = {};
var tiles = {};
var totalTiles = 0;
function processOneImage(imageFileName) {
    tiles[imageFileName] = [];
    return new Promise((resolve, reject) => {
        fs.createReadStream(imageFileName)
            .pipe(new PNG())
            .on('parsed', function () {
                if ((this.height % 8) !== 0 || (this.width % 8) !== 0) {
                    throw new Error("Image must have width and height be a multiple of 8x8");
                }
                var palette = new Palette();
                for (var y = 0; y < this.height; y++) {
                    for (var x = 0; x < this.width; x++) {
                        var index = (y * this.width + x) * 4;
                        palette.add(this.data[index], this.data[index + 1], this.data[index + 2], this.data[index + 3]);
                    }
                }
                palette.sort();
                var id = palette.toC();
                if (!palettes[id]) {
                    if (palettes.length >= 16) {
                        throw new Error("Cannot create more than 16 palettes");
                    }
                    palettes[id] = palette;
                }
                
                for (var y = 0; y < this.height / 8; y++) {
                    for (var x = 0; x < this.width / 8; x++) {
                        var tile = new Tile();
                        for (oy = 0; oy < 8; oy++) {
                            for (ox = 0; ox < 8; ox++) {
                                var index = ((y*8+oy) * this.width + (x*8+ox))*4;
                                var swatch = palette.lookup(this.data[index], this.data[index + 1], this.data[index + 2], this.data[index + 3]);
                                tile.setPixel(ox, oy, swatch);
                            }
                        }
                        tiles[imageFileName].push(tile);
                        totalTiles++;
                    }
                }
                resolve();
            });
    });
}
function internal() {
    if (options.inputImages.length) {
        return processOneImage(options.inputImages.shift()).then(processAllImages);
    } else {
    }
}
function processAllImages() {
    return new Promise((resolve, reject) => {
        var makePromise = function (file) {
            return () => processOneImage(file);
        };
        var p = Promise.resolve();
        while (options.inputImages.length) {
            p = p.then(makePromise(options.inputImages.shift()));
        }
        p = p.then(function () {
            var outDef = options.outputFileName.toUpperCase().replace(/[^0-9A-Za-z]/g, '_');
            var out = [`\#ifndef ${outDef}`, `\#define ${outDef}`];
            out.push(`#define ${options.outputConstPrefix}TILES ${totalTiles}`);
            out.push(`const uint32_t ${options.outputTilesName}[${options.outputConstPrefix}TILES][8] = {`);
            var files = [];
            Object.keys(tiles).sort().forEach(function (filename) {
                files.push('  // ' + filename + '\n' + tiles[filename].map(function (tile) {
                    return '  ' + tile.toC().split('\n').join('\n  ')
                }).join(',\n'));
            });
            out.push(files.join(',\n'));
            out.push(`}; // ${options.outputTilesName}`);
            keys = Object.keys(palettes);
            out.push(`#define ${options.outputConstPrefix}PALETTES ${keys.length}`);
            out.push(`const uint16_t ${options.outputPalettesName}[${options.outputConstPrefix}TILES][16] = {`);
            out.push(keys.map(function (key) {
                return '  ' + palettes[key].toC().split('\n').join('\n  ');
            }).join(',\n'));
            
            out.push(`}; // ${options.outputPalettesName}`);
            out.push(`#endif // ${outDef}`);
            resolve(out.join('\n'));
        });
        
    });
}

function saveResources(data) {
    fs.writeFileSync(options.outputFileName, data);
}
processAllImages().then(saveResources);