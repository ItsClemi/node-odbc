const pkg = require( "../../package.json" );


function getProxyConfig(): string
{
	return process.env.npm_config_https_proxy ||
		process.env.npm_config_proxy ||
		process.env.npm_config_http_proxy ||
		'';
}

function getUserAgent(): string
{
	return [
		'node/', process.version, ' ',
		'node-odbc-installer/', pkg.version
	].join( '' );
}


export function getProxy()
{
	let options = {
		rejectUnauthorized: false,
		timeout: 60000,
		headers: {
			'User-Agent': getUserAgent(),
		},
		proxy: '',
	};

	let proxyConfig = getProxyConfig();
	if( proxyConfig )
	{
		options.proxy = proxyConfig;
	}

	return options;
}