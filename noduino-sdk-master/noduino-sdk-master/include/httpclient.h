/*
 *  Copyright (c) 2015 - 2025 MaiKe Labs
 *
 *	This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 *	the Free Software Foundation, either version 3 of the License, or
 *	(at your option) any later version.
 *
 *	This program is distributed in the hope that it will be useful,
 *	but WITHOUT ANY WARRANTY; without even the implied warranty of
 *	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *	GNU General Public License for more details.
 *
 *	You should have received a copy of the GNU General Public License
 *	along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
*/
#ifndef __HTTPCLIENT_H__
#define __HTTPCLIENT_H__

#if defined(GLOBAL_DEBUG_ON)
#define HTTPCLIENT_DEBUG_ON
#endif
#if defined(HTTPCLIENT_DEBUG_ON)
#define HTTPCLIENT_DEBUG(format, ...) os_printf(format, ##__VA_ARGS__)
#else
#define HTTPCLIENT_DEBUG(format, ...)
#endif

#if defined(USES_SDK_BEFORE_V140)
#define espconn_send espconn_sent
#define espconn_secure_send espconn_secure_sent
#endif

/*
 * In case of TCP or DNS error the callback is called with this status.
 */
#define HTTP_STATUS_GENERIC_ERROR  (-1)

/*
 * Size of http responses that will cause an error.
 */
#define BUFFER_SIZE_MAX            (0x2000)

/*
 * Timeout of http request.
 */
#define HTTP_REQUEST_TIMEOUT_MS    (10000)

/*
 * "full_response":
 * 	 a string containing all response headers and the response body.
 *
 * "response_body" and "http_status": 
 *   extracted from "full_response" for convenience.
 *
 * A successful request corresponds to an HTTP status code of 200 (OK).
 * More info at http://en.wikipedia.org/wiki/List_of_HTTP_status_codes
 */
typedef void (*http_callback_t) (char *response_body, int http_status,
				 char *full_response);

/*
 * Call this function to skip URL parsing if the arguments are already in
 * separate variables.
 */
void ICACHE_FLASH_ATTR http_raw_request(const char *hostname, int port,
					bool secure, const char *method,
					const char *path, const char *headers,
					const char *post_data,
					http_callback_t callback_handle);

/*
 * Request data from URL use custom method.
 * The data should be encoded as any format.
 * Try:
 * http_request("http://httpbin.org/post", "OPTIONS", "Content-type: text/plain"
 * 				, "Hello world", http_callback_example);
 */
void ICACHE_FLASH_ATTR http_request(const char *url, const char *method,
				    const char *headers, const char *post_data,
				    http_callback_t callback_handle);

/*
 * Post data to a web form.
 * The data should be encoded as any format.
 * Try:
 * http_post("http://httpbin.org/post", "Content-type: application/json",
 * 				"{\"hello\": \"world\"}", http_callback_example);
 */
void ICACHE_FLASH_ATTR http_post(const char *url, const char *headers,
				 const char *post_data,
				 http_callback_t callback_handle);

/*
 * Download a web page from its URL.
 * Try:
 * http_get("http://wtfismyip.com/text", NULL, http_callback_example);
 */
void ICACHE_FLASH_ATTR http_get(const char *url, const char *headers,
				http_callback_t callback_handle);
/*
 * Delete a web page from its URL.
 * Try:
 * http_delete("http://wtfismyip.com/text", NULL, http_callback_example);
 */
void ICACHE_FLASH_ATTR http_delete(const char *url, const char *headers,
				   const char *post_data,
				   http_callback_t callback_handle);
/*
 * Update data to a web form.
 * The data should be encoded as any format.
 * Try:
 * http_put("http://httpbin.org/post", "Content-type: application/json",
 * 				"{\"hello\": \"world\"}", http_callback_example);
 */
void ICACHE_FLASH_ATTR http_put(const char *url, const char *headers,
				const char *post_data,
				http_callback_t callback_handle);

/*
 * Output on the UART.
 */
void http_callback_example(char *response, int http_status,
			   char *full_response);

#endif
