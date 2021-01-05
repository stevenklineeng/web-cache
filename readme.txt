Steven Kline

This program is a client-proxy server model where the client requests a website, and the proxy server then
checks its cache to see if the website is already on the system. If not, it then connects to the HTTP server
and recieves the files. The cache is then updated with this website, and the process is done again.

The functionality of this program is as follows: The client reads the input webpage and sends it to the 
proxy server. The proxy server then checks its cache to see if the website is already saved. If it isn't,
then it retrieves the information from the HTTP server, sends it to the client, and saves it in the cache.

To compile ::
	server : gcc web_cache_svr.c -o svr
	client : gcc web_cache_cli.c -o cli

To run ::
	server : ./svr <port number>
	client : ./cli <port number>