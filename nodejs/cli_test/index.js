#!/usr/bin/env node

const path = require('path');
const { marked } = require('marked');
const yargs = require('yargs/yargs');
const { hideBin } = require('yargs/helpers');
const { getPackageName } = require('./lib/name');
const { readMarkdownFileSync, writeHtmlFileSync } = require('./lib/file');

const { argv } = yargs(hideBin(process.argv))
	.option('name', {
		describe: 'Show CLI name'
	})
	.option('file', {
		describe: 'Specify markdown file path'
	})
	.option('out', {
		describe: 'Specify html file name',
		default: 'article.html'
	});

if (argv.name) {
	const name = getPackageName();
	console.log(name);
	process.exit(0);
}

const markdownStr = readMarkdownFileSync(path.resolve(__dirname, argv.file), { encoding: 'utf-8' });
const html = marked(markdownStr)

writeHtmlFileSync(path.resolve(__dirname, argv.out), html);

