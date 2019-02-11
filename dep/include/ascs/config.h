/*
 * config.h
 *
 *  Created on: 2016-9-14
 *      Author: youngwolf
 *		email: mail2tao@163.com
 *		QQ: 676218192
 *		Community on QQ: 198941541
 *
 * ascs top header file.
 *
 * license: http://think-async.com/ (current is www.boost.org/LICENSE_1_0.txt)
 *
 * Known issues:
 * 1. since 1.1.0 until 1.3, concurrentqueue is not a FIFO queue (it is by design), navigate to the following links for more details:
 *  https://github.com/cameron314/concurrentqueue/issues/6
 *  https://github.com/cameron314/concurrentqueue/issues/52
 *  if you're using concurrentqueue, please play attention, this is by design.
 * 2. since 1.1.5 until 1.2, heartbeat function cannot work properly between windows (at least win-10) and Ubuntu (at least Ubuntu-16.04).
 * 3. since 1.1.5 until 1.2, UDP doesn't support heartbeat because UDP doesn't support OOB data.
 * 4. since 1.1.5 until 1.2, SSL doesn't support heartbeat because SSL doesn't support OOB data.
 * 5. with old openssl (at least 0.9.7), ssl::client_socket_base and ssl_server_socket_base are not reusable, I'm not sure in which version,
 *    they became available, seems it's 1.0.0.
 * 6. since 1.0.0 until 1.3.0, async_write and async_read are not mutexed on the same socket, which is a violation of asio threading model.
 *
 * 2016.9.25	version 1.0.0
 * Based on st_asio_wrapper 1.2.0.
 * Directory structure refactoring.
 * Classes renaming, remove 'st_', 'tcp_' and 'udp_' prefix.
 * File renaming, remove 'st_asio_wrapper_' prefix.
 * Distinguish TCP and UDP related classes and files by tcp/udp namespace and tcp/udp directory.
 * Need c++14, if your compiler detected duplicated 'shared_mutex' definition, please define ASCS_HAS_STD_SHARED_MUTEX macro.
 * Need to define ASIO_STANDALONE and ASIO_HAS_STD_CHRONO macros.
 *
 * 2016.10.8	version 1.1.0
 * Support concurrent queue (https://github.com/cameron314/concurrentqueue), it's lock-free.
 * Define ASCS_USE_CONCURRENT_QUEUE macro to use your personal message queue.
 * Define ASCS_USE_CONCURRE macro to use concurrent queue, otherwise ascs::list will be used as the message queue.
 * Drop original congestion control (because it cannot totally resolve dead loop) and add a semi-automatic congestion control.
 * Demonstrate how to use the new semi-automatic congestion control (echo_server, echo_client, pingpong_server and pingpong_client).
 * Drop post_msg_buffer and corresponding functions (like post_msg()) and timer (ascs::socket::TIMER_HANDLE_POST_BUFFER).
 * Optimize locks on message sending and dispatching.
 * Add enum shutdown_states.
 * Rename class ascs::std_list to ascs::list.
 * ascs::timer now can be used independently.
 * Add a new type ascs::timer::tid to represent timer ID.
 * Add a new packer--fixed_length_packer.
 * Add a new class--message_queue.
 *
 * 2016.10.16	version 1.1.1
 * Support non-lock queue, it's totally not thread safe and lock-free, it can improve IO throughput with particular business.
 * Demonstrate how and when to use non-lock queue as the input and output message buffer.
 * Queues (and their internal containers) used as input and output message buffer are now configurable (by macros or template arguments).
 * New macros--ASCS_INPUT_QUEUE, ASCS_INPUT_CONTAINER, ASCS_OUTPUT_QUEUE and ASCS_OUTPUT_CONTAINER.
 * Drop macro ASCS_USE_CONCURRENT_QUEUE, rename macro ASCS_USE_CONCURRE to ASCS_HAS_CONCURRENT_QUEUE.
 * In contrast to non_lock_queue, split message_queue into lock_queue and lock_free_queue.
 * Move container related classes and functions from base.h to container.h.
 * Improve efficiency in scenarios of low throughput like pingpong test.
 * Replaceable packer/unpacker now support replaceable_buffer (an alias of auto_buffer) and shared_buffer to be their message type.
 * Move class statistic and obj_with_begin_time out of ascs::socket to reduce template tiers.
 *
 * 2016.11.1	version 1.1.2
 * Fix bug: ascs::list cannot be moved properly via moving constructor.
 * Use ASCS_DELAY_CLOSE instead of ASCS_ENHANCED_STABILITY macro to control delay close duration,
 *  0 is an equivalent of defining ASCS_ENHANCED_STABILITY, other values keep the same meanings as before.
 * Move ascs::socket::closing related logic to ascs::object.
 * Make ascs::socket::id(uint_fast64_t) private to avoid changing IDs by users.
 * Call close at the end of shutdown function, just for safety.
 * Add move capture in lambda.
 * Optimize lambda expressions.
 *
 * 2016.11.13	version 1.1.3
 * Introduce lock-free mechanism for some appropriate logics (many requesters, only one can succeed, others will fail rather than wait).
 * Remove all mutex (except mutex in object_pool, service_pump, lock_queue and udp::socket).
 * Sharply simplified timer class.
 *
 * 2016.12.6	version 1.1.4
 * Drop unnecessary macro definition (ASIO_HAS_STD_CHRONO).
 * Simplify header files' dependence.
 * Add Visual C++ solution and project files (Visual C++ 14.0).
 * Monitor time consumptions for message packing and unpacking.
 * Fix bug: pop_first_pending_send_msg and pop_first_pending_recv_msg cannot work.
 *
 * 2017.1.1		version 1.1.5
 * Support heartbeat (via OOB data), see ASCS_HEARTBEAT_INTERVAL macro for more details.
 * Support scatter-gather buffers when receiving messages, this feature needs modification of i_unpacker, you must explicitly define
 *  ASCS_SCATTERED_RECV_BUFFER macro to open it, this is just for compatibility.
 * Simplify lock-free mechanism and use std::atomic_flag instead of std::atomic_size_t.
 * Optimize container insertion (use series of emplace functions instead).
 * Demo echo_client support alterable number of sending thread (before, it's a hard code 16).
 * Fix bug: In extreme cases, messages may get starved in receive buffer and will not be dispatched until arrival of next message.
 * Fix bug: In extreme cases, messages may get starved in send buffer and will not be sent until arrival of next message.
 * Fix bug: Sometimes, connector_base cannot reconnect to the server after link broken.
 *
 * ===============================================================
 * 2017.5.30	version 1.2.0
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * Virtual function reset_state in i_packer and i_unpacker have been renamed to reset.
 * Virtual function is_send_allowed has been renamed to is_ready, it also means ready to receive messages
 *  since message sending cannot be suspended any more.
 * Virtual function on_msg_handle has been changed, the link_down variable will not be presented any more.
 * Interface i_server::del_client has been renamed to i_server::del_socket.
 * Function inner_packer and inner_unpacker have been renamed to packer and unpacker.
 * All add_client functions have been renamed to add_socket.
 *
 * HIGHLIGHT:
 * Support object restoration (on server side), see macro ASCS_RESTORE_OBJECT for more details.
 * Re-implement heartbeat function, use user data (so need packer and unpacker's support) instead of OOB, now there will be no any
 *  limitations for using heartbeat.
 * Refactor and optimize ssl objects, now ssl::connector_base and ssl::server_socket_base are reusable,
 *  just need you to define macro ASCS_REUSE_SSL_STREAM.
 *
 * FIX:
 * Before on_close() to be called, socket::start becomes available (so user can call it falsely).
 * If a timer failed or stopped by callback, its status not set properly (should be set to TIMER_CANCELED).
 * Make ssl shutting down thread safe.
 * Make reconnecting after all async invocations (like object reusing or restoration).
 *
 * ENHANCEMENTS:
 * Virtual function i_packer::pack_heartbeat been introduced to support heartbeat function.
 * Interface i_server::restore_socket been added, see macro ASCS_RESTORE_OBJECT for more details.
 * Be able to manually start heartbeat function without defining macro ASCS_HEARTBEAT_INTERVAL, see demo echo_server for more details.
 * Support sync mode when sending messages, it's also a type of congestion control like safe_send_msg.
 * Expand enum tcp::socket::shutdown_states, now it's able to represent all SOCKET status (connected, shutting down and broken),
 *  so rename it to link_status.
 * Enhance class timer (function is_timer and another stop_all_timer been added).
 * Support all buffer types that asio supported when receiving messages, use macro ASCS_RECV_BUFFER_TYPE (it's the only way) to define the buffer type,
 *  it's effective for both TCP and UDP.
 *
 * DELETION:
 * Drop ASCS_HAS_STD_SHARED_MUTEX macro.
 * Drop ASCS_DISCARD_MSG_WHEN_LINK_DOWN macro and related logic, because it brings complexity and race condition,
 *  and are not very useful.
 * Drop socket::is_closable, tcp::connector_base will overwrite socket::on_close to implement reconnecting mechanism.
 * Not support pausing message sending and dispatching any more, because they bring complexity and race condition,
 *  and are not very useful.
 *
 * REFACTORING:
 * Move heartbeat function from connector_base and server_socket_base to socket, so introduce new virtual function
 *  virtual void on_heartbeat_error() to socket, subclass need to implement it.
 * Move async shutdown function from connector_base and server_socket_base to socket, so introduce new virtual function
 *  virtual void on_async_shutdown_error() to socket, subclass need to implement it.
 * Move handshake from ssl::server_base to ssl::server_socket_base.
 *
 * REPLACEMENTS:
 * Use std::mutex instead of std::shared_mutex (the former is more efficient in ascs' usage scenario), thus c++11 is enough for ascs.
 * Move macro ASCS_SCATTERED_RECV_BUFFER from ascs to ascs::ext, because it doesn't belong to ascs any more after introduced macro ASCS_RECV_BUFFER_TYPE.
 * Move directory include/ascs/ssl into directory include/ascs/tcp/, because ssl is based on ascs::tcp.
 *
 * ===============================================================
 * 2017.6.19	version 1.2.1
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 *
 * HIGHLIGHT:
 *
 * FIX:
 * Fix race condition on member variable last_send_msg in tcp::socket_base.
 *
 * ENHANCEMENTS:
 * Optimize reconnecting mechanism.
 * Enhance class timer.
 *
 * DELETION:
 *
 * REFACTORING:
 *
 * REPLACEMENTS:
 * Rename connector_base and ssl::connector_base to client_socket_base and ssl::client_socket_base, the former is still available, but is just an alias.
 *
 * ===============================================================
 * 2017.7.9		version 1.2.2
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * No error_code will be presented anymore when call io_context::run, suggest to define macro ASCS_ENHANCED_STABILITY.
 *
 * HIGHLIGHT:
 * Add two demos for concurrent test.
 * Support unstripped message (take the default unpacker for example, it will not strip message header in parse_msg), this feature is disabled by default,
 *  you can call i_unpacker's void stripped(bool) function to enable it.
 *  udp::i_unpacker doesn't have this feature, it always and only support unstripped message.
 *
 * FIX:
 * Fix reconnecting mechanism in demo ssl_test.
 *
 * ENHANCEMENTS:
 * Truly support asio 1.11 (don't use deprecated functions and classes any more), and of course, asio 1.10 will be supported too.
 *
 * DELETION:
 *
 * REFACTORING:
 *
 * REPLACEMENTS:
 * Use mutable_buffer and const_buffer instead of mutable_buffers_1 and const_buffers_1 if possible, this can gain some performance improvement.
 * Call force_shutdown instead of graceful_shutdown in tcp::client_base::uninit().
 *
 * ===============================================================
 * 2017.7.23	version 1.2.3
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * i_server has been moved from ascs to ascs::tcp.
 *
 * HIGHLIGHT:
 * Support decreasing (increasing already supported) the number of service thread at runtime by defining ASCS_DECREASE_THREAD_AT_RUNTIME macro,
 *  suggest to define ASCS_AVOID_AUTO_STOP_SERVICE macro too.
 *
 * FIX:
 * Always directly shutdown ssl::client_socket_base if macro ASCS_REUSE_SSL_STREAM been defined.
 * Make queue::clear and swap thread-safe if possible.
 *
 * ENHANCEMENTS:
 * Optimized and simplified auto_buffer, shared_buffer and ext::basic_buffer.
 * Optimized class obj_with_begin_time.
 * Not use sending buffer (send_msg_buffer) if possible.
 * Reduced stopped() invocation (because it needs locks).
 * Introduced asio::io_service::work (asio::executor_work_guard) by defining ASCS_AVOID_AUTO_STOP_SERVICE macro.
 * Add function service_pump::service_thread_num to fetch the real number of service thread (must define ASCS_DECREASE_THREAD_AT_RUNTIME macro).
 *
 * DELETION:
 * Not support Visual C++ 11.0 (2012) any more, use st_asio_wrapper instead.
 *
 * REFACTORING:
 * Move all deprecated classes (connector_base, client_base, service_base) to alias.h
 * Refactor the mechanism of message sending.
 *
 * REPLACEMENTS:
 * Rename tcp::client_base to tcp::multi_client_base, ext::tcp::client to ext::tcp::multi_client, udp::service_base to udp::multi_service_base,
 *  ext::udp::service to ext::udp::multi_service. Old ones are still available, but have became alias.
 *
 * ===============================================================
 * 2017.9.17	version 1.2.4
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * Function object_pool::invalid_object_pop only pop obsoleted objects with no additional reference.
 * socket::stat.last_recv_time will not be updated before tcp::socket_base::on_connect any more.
 * For ssl socket, on_handshake will be invoked before on_connect (before, on_connect is before on_handshake).
 *
 * HIGHLIGHT:
 *
 * FIX:
 * If start the same timer and return false in the timer's call_back, its status will be set to TIMER_CANCELED (the right value should be TIMER_OK).
 * In old compilers (for example gcc 4.7), std::list::splice needs a non-constant iterator as the insert point.
 * If call stop_service after service_pump stopped, timer TIMER_DELAY_CLOSE will be left behind and be triggered after the next start_service,
 *  this will bring disorders to ascs::socket.
 *
 * ENHANCEMENTS:
 * During congestion controlling, retry interval can be changed at runtime, you can use this feature for performance tuning,
 *  see macro ASCS_MSG_HANDLING_INTERVAL_STEP1 and ASCS_MSG_HANDLING_INTERVAL_STEP2 for more details.
 * Avoid decreasing the number of service thread to less than one.
 * Add a helper function object_pool::get_statistic.
 * Add another overload of function object_pool::invalid_object_pop.
 * Introduce asio::defer to object, be careful to use it.
 * Add link's break time and establish time to the statistic object.
 * Move virtual function client_socket_base::on_connect to tcp::socket_base, so server_socket_base will have it too (and ssl sockets).
 *
 * DELETION:
 * Drop useless variables which need macro ASCS_DECREASE_THREAD_AT_RUNTIME to be defined.
 *
 * REFACTORING:
 * Move variable last_send_time and last_recv_time from ascs::socket to ascs::socket::stat (a statistic object).
 * Move common operations in client_socket_base::do_start and server_socket_base::do_start to tcp::socket_base::do_start and socket::do_start.
 *
 * REPLACEMENTS:
 * Always use io_context instead of io_service (before asio 1.11, io_context will be a typedef of io_service).
 *
 * ===============================================================
 * 2017.9.28	version 1.2.5
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 *
 * HIGHLIGHT:
 *
 * FIX:
 * Support unmovable buffers (for example: a very short std::string).
 * begin_time not inherited while accessing concurrent queue.
 * Eliminate race condition in udp::socket_base.
 * Eliminate race condition in demo file_client.
 * Avoid division by zero error in demo file_client.
 *
 * ENHANCEMENTS:
 *
 * DELETION:
 *
 * REFACTORING:
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2018.4.10	version 1.2.6
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * Do reconnecting in client_socket_base::after_close rather than in client_socket_base::on_close.
 *
 * HIGHLIGHT:
 *
 * FIX:
 * Reconnecting may happen in ascs::socket::reset, it's not a right behavior.
 *
 * ENHANCEMENTS:
 * Add ascs::socket::after_close virtual function, a good case for using it is to reconnect the server in client_socket_base.
 *
 * DELETION:
 *
 * REFACTORING:
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2018.5.20	version 1.3.0
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * Not support concurrent queue any more.
 * Not support sync sending mode any more.
 * Explicitly need macro ASCS_PASSIVE_RECV to gain the ability of changing the unpacker at runtime.
 * Function disconnect, force_shutdown and graceful_shutdown in udp::socket_base will now be performed asynchronously.
 * Not support macro ASCS_FORCE_TO_USE_MSG_RECV_BUFFER any more, which means now we have the behavior as this macro always defined,
 *  thus, virtual function ascs::socket::on_msg() is useless and also has been deleted.
 * statistic.handle_time_2_sum has been renamed to handle_time_sum.
 * Macro ASCS_MSG_HANDLING_INTERVAL_STEP1 has been renamed to ASCS_MSG_RESUMING_INTERVAL.
 * Macro ASCS_MSG_HANDLING_INTERVAL_STEP2 has been renamed to ASCS_MSG_HANDLING_INTERVAL.
 * ascs::socket::is_sending_msg() has been renamed to is_sending().
 * ascs::socket::is_dispatching_msg() has been renamed to is_dispatching().
 * typedef ascs::socket::in_container_type has been renamed to in_queue_type.
 * typedef ascs::socket::out_container_type has been renamed to out_queue_type.
 * Wipe default value for parameter can_overflow in function send_msg, send_native_msg, safe_send_msg, safe_send_native_msg,
 *  broadcast_msg, broadcast_native_msg, safe_broadcast_msg and safe_broadcast_native_msg, this is because we added template parameter to some of them,
 *  and the compiler will complain (ambiguity) if we omit the can_overflow parameter. So take send_msg function for example, if you omitted can_overflow
 *  before, then in 1.3, you must supplement it, like send_msg(...) -> send_msg(..., false).
 *
 * HIGHLIGHT:
 * After introduced asio::io_context::strand (which is required, see FIX section for more details), we wiped two atomic in ascs::socket.
 * Introduced macro ASCS_DISPATCH_BATCH_MSG, then all messages will be dispatched via on_handle_msg with a variable-length container.
 *
 * FIX:
 * Wiped race condition between async_read and async_write on the same ascs::socket, so sync sending mode will not be supported any more.
 *
 * ENHANCEMENTS:
 * Explicitly define macro ASCS_PASSIVE_RECV to gain the ability of changing the unpacker at runtime.
 * Add function ascs::socket::is_reading() if macro ASCS_PASSIVE_RECV been defined, otherwise, the socket will always be reading.
 * Add function ascs::socket::is_recv_buffer_available(), you can use it before calling recv_msg() to avoid receiving buffer overflow.
 * Add typedef ascs::socket::in_container_type to represent the container type used by in_queue_type (sending buffer).
 * Add typedef ascs::socket::out_container_type to represent the container type used by out_queue_type (receiving buffer).
 * Generalize function send_msg, send_native_msg, safe_send_msg, safe_send_native_msg, broadcast_msg, broadcast_native_msg,
 *  safe_broadcast_msg and safe_broadcast_native_msg.
 *
 * DELETION:
 * Deleted macro ASCS_SEND_BUFFER_TYPE.
 * Deleted virtual function bool ascs::socket::on_msg().
 * Not support sync sending mode any more, so we reduced an atomic object in ascs::socket.
 *
 * REFACTORING:
 * If you want to change unpacker at runtime, first, you must define macro ASCS_PASSIVE_RECV, second, you must call ascs::socket::recv_msg,
 *  see file_client for more details.
 * Class object has been split into executor and tracked_executor, object_pool use the former, and ascs::socket use the latter.
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2018.8.1		version 1.3.1
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * The data type of timer ID has been changed from unsigned char to unsigned short.
 *
 * HIGHLIGHT:
 * Support Cygwin and Mingw.
 * Dynamically allocate timers when needed (multi-threading related behaviors kept as before, so we must introduce a mutex for ascs::timer object).
 *
 * FIX:
 *
 * ENHANCEMENTS:
 * The range of timer ID has been expanded from [0, 256) to [0, 65536).
 * Add new macro ASCS_ALIGNED_TIMER to align timers.
 *
 * DELETION:
 *
 * REFACTORING:
 * Realigned member variables for ascs::socket to save a few memory.
 * Make demos more easier to use.
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2018.8.21	version 1.3.2
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * If macro ASCS_PASSIVE_RECV been defined, you may receive empty messages in on_msg() or on_msg_handle() and sync_recv_msg(), this makes you always having
 *  the chance to call recv_msg().
 * i_unpacker has been moved from namespace ascs::tcp and ascs::udp to namespace ascs, and the signature of ascs::udp::i_unpacker::parse_msg
 *  has been changed to obey ascs::tcp::i_unpacker::parse_msg.
 *
 * HIGHLIGHT:
 * Fully support sync message sending and receiving (even be able to mix with async message sending and receiving without any limitations), but please note
 *  that this feature will slightly impact efficiency even if you always use async message sending and receiving, so only open this feature when really needed.
 *
 * FIX:
 * Fix race condition when aligning timers, see macro ASCS_ALIGNED_TIMER for more details.
 * Fix error check for UDP on cygwin and mingw.
 * Fix bug: demo file_client may not be able to receive all content of the file it required if you get more than one file in a single request.
 *
 * ENHANCEMENTS:
 * Add new macro ASCS_SYNC_SEND and ASCS_SYNC_RECV to support sync message sending and receiving.
 *
 * DELETION:
 *
 * REFACTORING:
 * i_unpacker has been moved from namespace ascs::tcp and ascs::udp to namespace ascs, and the signature of ascs::udp::i_unpacker::parse_msg
 *  has been changed to obey ascs::tcp::i_unpacker::parse_msg, the purpose of this change is to make socket::sync_recv_msg() can be easily
 *  implemented, otherwise, sync_recv_msg() must be implemented by tcp::socket_base and udp::socket_base respectively.
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2018.10.1	version 1.3.3
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * With sync message sending, if you rewrite virtual function on_close, you must also call super class' on_close.
 *
 * HIGHLIGHT:
 * Support sync message dispatching, it's like previous on_msg() callback but with a message container instead of a message (and many other
 *  differences, see macro ASCS_SYNC_DISPATCH for more details), and we also name it on_msg().
 * Support timed waiting when doing sync message sending and receiving.
 * Support pre-creating server sockets in server_base (before accepting connections), this is very useful if creating server socket is very expensive.
 *
 * FIX:
 * Fix spurious awakenings when doing sync message sending and receiving.
 * Fix statistics for batch message dispatching.
 *
 * ENHANCEMENTS:
 * Add virtual function find_socket to interface i_server.
 * Support timed waiting when doing sync message sending and receiving, please note that after timeout, sync operations can succeed in the future because
 *  ascs uses async operations to simulate sync operations. for sync receiving, missed messages can be dispatched via the next sync receiving, on_msg (if
 *  macro ASCS_SYNC_DISPATCH been defined) and / or on_msg_handle.
 * Add virtual function server_socket_base::async_accept_num() to support pre-creating server sockets at runtime, by default, it returns ASCS_ASYNC_ACCEPT_NUM,
 *  see demo concurrent_server and macro ASCS_ASYNC_ACCEPT_NUM for more details.
 *
 * DELETION:
 * Not support concurrent queue any more, so delete macro ASCS_HAS_CONCURRENT_QUEUE and class lock_free_queue.
 *
 * REFACTORING:
 * Hide member variables as many as possible for developers.
 *
 * REPLACEMENTS:
 *
 * ===============================================================
 * 2019.1.1		version 1.3.4
 *
 * SPECIAL ATTENTION (incompatible with old editions):
 * The virtual function socket::on_send_error has been moved to tcp::socket_base and udp::socket_base.
 * The signature of virtual function socket::on_send_error has been changed, a container holding messages that were failed to send will be provided.
 * Failure of binding or listening in server_base will not stop the service_pump any more.
 * Virtual function client_socket_base::prepare_reconnect() now only control the retry times and delay time after reconnecting failed.
 *
 * HIGHLIGHT:
 *
 * FIX:
 * If give up connecting (prepare_reconnect returns -1 or call close_reconnect), ascs::socket::started() still returns true (should be false).
 *
 * ENHANCEMENTS:
 * Expose server_base's acceptor via next_layer().
 * Prefix suffix packer and unpacker support heartbeat.
 * New demo socket_management demonstrates how to manage sockets if you use other keys rather than the original id.
 * Control reconnecting more flexibly, see function client_socket_base::open_reconnect and client_socket_base::close_reconnect for more details.
 * client_socket_base support binding to a specific local address.
 *
 * DELETION:
 *
 * REFACTORING:
 *
 * REPLACEMENTS:
 *
 */

#ifndef _ASCS_CONFIG_H_
#define _ASCS_CONFIG_H_

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#define ASCS_VER		10304	//[x]xyyzz -> [x]x.[y]y.[z]z
#define ASCS_VERSION	"1.3.4"

//asio and compiler check
#ifdef _MSC_VER
	#define ASCS_SF "%Iu" //format used to print 'size_t'
	static_assert(_MSC_VER >= 1800, "ascs needs Visual C++ 12.0 (2013) or higher.");
#elif defined(__GNUC__)
	#ifdef __clang__
		static_assert(__clang_major__ > 3 || (__clang_major__ == 3 && __clang_minor__ >= 1), "ascs needs Clang 3.1 or higher.");
	#else
		static_assert(__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6), "ascs needs GCC 4.6 or higher.");
	#endif

	#if !defined(__GXX_EXPERIMENTAL_CXX0X__) && (!defined(__cplusplus) || __cplusplus < 201103L)
		#error ascs needs c++11 or higher.
	#endif

	#define ASCS_SF "%zu" //format used to print 'size_t'
	#if defined(__x86_64__) && !defined(__MINGW64__) //terrible mingw
	#define ASCS_LLF "%lu" //format used to print 'uint_fast64_t'
	#endif
#else
	#error ascs only support Visual C++, GCC and Clang.
#endif

#ifndef ASCS_LLF
#define ASCS_LLF "%llu" //format used to print 'uint_fast64_t'
#endif

static_assert(ASIO_VERSION >= 101001, "ascs needs asio 1.10.1 or higher.");

#if ASIO_VERSION < 101100
namespace asio {typedef io_service io_context;}
#define make_strand_handler(S, F) S.wrap(F)
#else
#define make_strand_handler(S, F) asio::bind_executor(S, F)
#endif
//asio and compiler check

//configurations

#ifndef ASCS_SERVER_IP
#define ASCS_SERVER_IP			"127.0.0.1"
#endif
#ifndef ASCS_SERVER_PORT
#define ASCS_SERVER_PORT		5050
#endif
static_assert(ASCS_SERVER_PORT > 0, "server port must be bigger than zero.");

//msg send and recv buffer's maximum size (list::size()), corresponding buffers are expanded dynamically, which means only allocate memory when needed.
#ifndef ASCS_MAX_MSG_NUM
#define ASCS_MAX_MSG_NUM		1024
#endif
static_assert(ASCS_MAX_MSG_NUM > 0, "message capacity must be bigger than zero.");

//buffer (on stack) size used when writing logs.
#ifndef ASCS_UNIFIED_OUT_BUF_NUM
#define ASCS_UNIFIED_OUT_BUF_NUM	2048
#endif

//use customized log system (you must provide unified_out::fatal_out/error_out/warning_out/info_out/debug_out)
//#define ASCS_CUSTOM_LOG

//don't write any logs.
//#define ASCS_NO_UNIFIED_OUT

//if defined, service_pump will catch exceptions for asio::io_context::run().
//#define ASCS_ENHANCED_STABILITY

//if defined, asio::steady_timer will be used in ascs::timer, otherwise, asio::system_timer will be used.
//#define ASCS_USE_STEADY_TIMER

//after this duration, this socket can be freed from the heap or reused,
//you must define this macro as a value, not just define it, the value means the duration, unit is second.
//a value equal to zero will cause ascs to use a mechanism to guarantee 100% safety when reusing or freeing this socket,
//ascs will hook all async calls to avoid this socket to be reused or freed before all async calls finish
//or been interrupted (of course, this mechanism will slightly impact efficiency).
#ifndef ASCS_DELAY_CLOSE
#define ASCS_DELAY_CLOSE	0 //seconds, guarantee 100% safety when reusing or freeing this socket
#endif
static_assert(ASCS_DELAY_CLOSE >= 0, "delay close duration must be bigger than or equal to zero.");

//full statistic include time consumption, or only numerable informations will be gathered
//#define ASCS_FULL_STATISTIC

//after every msg sent, call ascs::socket::on_msg_send()
//#define ASCS_WANT_MSG_SEND_NOTIFY

//after sending buffer became empty, call ascs::socket::on_all_msg_send()
//#define ASCS_WANT_ALL_MSG_SEND_NOTIFY

//max number of objects object_pool can hold.
#ifndef ASCS_MAX_OBJECT_NUM
#define ASCS_MAX_OBJECT_NUM	4096
#endif
static_assert(ASCS_MAX_OBJECT_NUM > 0, "object capacity must be bigger than zero.");

//if defined, objects will never be freed, but remain in object_pool waiting for reuse.
//#define ASCS_REUSE_OBJECT

//this macro has the same effects as macro ASCS_REUSE_OBJECT (it will overwrite the latter), except:
//reuse will not happen when create new connections, but just happen when invoke i_server::restore_socket.
//you may ask, for what purpose we introduced this feature?
//consider following situation:
//if a specific link is down, and the client has reconnected to the server, on the server side, how does the new server_socket_base
//restore all user data (because you don't want to nor need to reestablish them) and keep its id?
//before this feature been introduced, it's almost impossible.
//according to above explanation, we know that:
//1. like object pool, only objects in invalid_object_can can be restored;
//2. client need to inform server_socket_base the former id (or something else which can be used to calculate the former id
//   on the server side) after reconnected to the server;
//3. this feature needs user's support (send former id to server side on client side, invoke i_server::restore_socket in server_socket_base);
//4. do not define this macro on client side nor for UDP.
//#define ASCS_RESTORE_OBJECT

//define ASCS_REUSE_OBJECT or ASCS_RESTORE_OBJECT macro will enable object pool, all objects in invalid_object_can will
// never be freed, but kept for reuse, otherwise, object_pool will free objects in invalid_object_can automatically and periodically,
//ASCS_FREE_OBJECT_INTERVAL means the interval, unit is second, see invalid_object_can in object_pool class for more details.
#if !defined(ASCS_REUSE_OBJECT) && !defined(ASCS_RESTORE_OBJECT)
	#ifndef ASCS_FREE_OBJECT_INTERVAL
	#define ASCS_FREE_OBJECT_INTERVAL	60 //seconds
	#elif ASCS_FREE_OBJECT_INTERVAL <= 0
		#error free object interval must be bigger than zero.
	#endif
#endif

//define ASCS_CLEAR_OBJECT_INTERVAL macro to let object_pool to invoke clear_obsoleted_object() automatically and periodically
//this feature may affect performance with huge number of objects, so re-write server_socket_base::on_recv_error and invoke object_pool::del_object()
//is recommended for long-term connection system, but for short-term connection system, you are recommended to open this feature.
//you must define this macro as a value, not just define it, the value means the interval, unit is second
//#define ASCS_CLEAR_OBJECT_INTERVAL		60 //seconds
#if defined(ASCS_CLEAR_OBJECT_INTERVAL) && ASCS_CLEAR_OBJECT_INTERVAL <= 0
	#error clear object interval must be bigger than zero.
#endif

//IO thread number
//listening, msg sending and receiving, msg handling (on_msg() and on_msg_handle()), all timers (include user timers) and other asynchronous calls (from executor)
//keep big enough, no empirical value I can suggest, you must try to find it out in your own environment
#ifndef ASCS_SERVICE_THREAD_NUM
#define ASCS_SERVICE_THREAD_NUM	8
#endif
static_assert(ASCS_SERVICE_THREAD_NUM > 0, "service thread number be bigger than zero.");

//graceful shutdown must finish within this duration, otherwise, socket will be forcedly shut down.
#ifndef ASCS_GRACEFUL_SHUTDOWN_MAX_DURATION
#define ASCS_GRACEFUL_SHUTDOWN_MAX_DURATION	5 //seconds
#endif
static_assert(ASCS_GRACEFUL_SHUTDOWN_MAX_DURATION > 0, "graceful shutdown duration must be bigger than zero.");

//if connecting (or reconnecting) failed, delay how much milliseconds before reconnecting, negative value means stop reconnecting,
//you can also rewrite tcp::client_socket_base::prepare_reconnect(), and return a negative value.
#ifndef ASCS_RECONNECT_INTERVAL
#define ASCS_RECONNECT_INTERVAL	500 //millisecond(s)
#endif

//how many async_accept delivery concurrently, an equivalent way is to rewrite virtual function server_socket_base::async_accept_num().
//server_base will pre-create ASCS_ASYNC_ACCEPT_NUM server sockets before starting accepting connections, but we may don't want so many
// async_accept delivery all the time, what should we do?
//the answer is, we can rewrite virtual function server_socket_base::start_next_accept() and decide whether to continue the async accepting or not,
// for example, you normally have about 1000 connections, so you define ASCS_ASYNC_ACCEPT_NUM as 1000, and:
// 1. rewrite virtual function server_socket_base::on_accept() and count how many connections already established;
// 2. rewrite virtual function server_socket_base::start_next_accept(), if we have already established 990 connections (or even more),
//    then call server_socket_base::start_next_accept() to continue the async accepting, otherwise, just return (to stop the async accepting).
// after doing that, the server will not create server sockets during accepting the 1st to 990th connections, and after the 990th connection,
// the server will always keep 10 async accepting (which also means after accepted a new connection, a new server socket will be created),
// see demo concurrent_server for more details.
//this feature is very useful if creating server socket is very expensive (which means creating server socket may affect the process or even
// the system seriously) and the total number of connections is basically stable (or you have a limitation for the number of connections).
//even creating server socket is very cheap, augmenting this macro can speed up connection accepting especially when many connecting request flood in.
#ifndef ASCS_ASYNC_ACCEPT_NUM
#define ASCS_ASYNC_ACCEPT_NUM	16
#endif
static_assert(ASCS_ASYNC_ACCEPT_NUM > 0, "async accept number must be bigger than zero.");

//in server_base::set_server_addr and set_local_addr, if the IP is empty, ASCS_(TCP/UDP)_DEFAULT_IP_VERSION will define the IP version,
// or the IP version will be deduced by the IP address.
//asio::ip::(tcp/udp)::v4() means ipv4 and asio::ip::(tcp/udp)::v6() means ipv6.
#ifndef ASCS_TCP_DEFAULT_IP_VERSION
#define ASCS_TCP_DEFAULT_IP_VERSION asio::ip::tcp::v4()
#endif
#ifndef ASCS_UDP_DEFAULT_IP_VERSION
#define ASCS_UDP_DEFAULT_IP_VERSION asio::ip::udp::v4()
#endif

//close port reuse
//#define ASCS_NOT_REUSE_ADDRESS

#ifndef ASCS_INPUT_QUEUE
#define ASCS_INPUT_QUEUE lock_queue
#endif
#ifndef ASCS_INPUT_CONTAINER
#define ASCS_INPUT_CONTAINER list
#endif
#ifndef ASCS_OUTPUT_QUEUE
#define ASCS_OUTPUT_QUEUE lock_queue
#endif
#ifndef ASCS_OUTPUT_CONTAINER
#define ASCS_OUTPUT_CONTAINER list
#endif
//we also can control the queues (and their containers) via template parameters on class 'client_socket_base'
//'server_socket_base', 'ssl::client_socket_base' and 'ssl::server_socket_base'.
//we even can let a socket to use different queue (and / or different container) for input and output via template parameters.

//buffer type used when receiving messages (unpacker's prepare_next_recv() need to return this type)
#ifndef ASCS_RECV_BUFFER_TYPE
	#if ASIO_VERSION >= 101100
	#define ASCS_RECV_BUFFER_TYPE asio::mutable_buffer
	#else
	#define ASCS_RECV_BUFFER_TYPE asio::mutable_buffers_1
	#endif
#endif

#ifndef ASCS_HEARTBEAT_INTERVAL
#define ASCS_HEARTBEAT_INTERVAL	0 //second(s), disable heartbeat by default, just for compatibility
#endif
//at every ASCS_HEARTBEAT_INTERVAL second(s):
// 1. tcp::socket_base will send an heartbeat if no messages been sent within this interval,
// 2. tcp::socket_base will check the link's connectedness, see ASCS_HEARTBEAT_MAX_ABSENCE macro for more details.
//less than or equal to zero means disable heartbeat, then you can send and check heartbeat with you own logic by calling
//tcp::socket_base::check_heartbeat (do above steps one time) or tcp::socket_base::start_heartbeat (do above steps regularly).

#ifndef ASCS_HEARTBEAT_MAX_ABSENCE
#define ASCS_HEARTBEAT_MAX_ABSENCE	3 //times of ASCS_HEARTBEAT_INTERVAL
#endif
static_assert(ASCS_HEARTBEAT_MAX_ABSENCE > 0, "heartbeat absence must be bigger than zero.");
//if no any messages (include heartbeat) been received within ASCS_HEARTBEAT_INTERVAL * ASCS_HEARTBEAT_MAX_ABSENCE second(s), shut down the link.

//#define ASCS_REUSE_SSL_STREAM
//if you need ssl::client_socket_base to be able to reconnect the server, or to open object pool in ssl::object_pool, you must define this macro.
//I tried many ways, only one way can make asio::ssl::stream reusable, which is:
// don't call any shutdown functions of asio::ssl::stream, just call asio::ip::tcp::socket's shutdown function,
// this seems not a normal procedure, but it works, I believe that asio's defect caused this problem.

//#define ASCS_AVOID_AUTO_STOP_SERVICE
//wrap service_pump with asio::io_service::work (asio::executor_work_guard), then it will never run out until you explicitly call stop_service().

//#define ASCS_DECREASE_THREAD_AT_RUNTIME
//enable decreasing service thread at runtime.

#ifndef ASCS_MSG_RESUMING_INTERVAL
#define ASCS_MSG_RESUMING_INTERVAL	50 //milliseconds
#endif
static_assert(ASCS_MSG_RESUMING_INTERVAL >= 0, "the interval of msg resuming must be bigger than or equal to zero.");
//msg receiving
//if receiving buffer is overflow, message receiving will stop and resume after the buffer becomes available, 
//this is the interval of receiving buffer checking.
//this value can be changed via ascs::socket::msg_resuming_interval(size_t) at runtime.

#ifndef ASCS_MSG_HANDLING_INTERVAL
#define ASCS_MSG_HANDLING_INTERVAL	50 //milliseconds
#endif
static_assert(ASCS_MSG_HANDLING_INTERVAL >= 0, "the interval of msg handling must be bigger than or equal to zero.");
//msg handling
//call on_msg_handle, if failed, retry it after ASCS_MSG_HANDLING_INTERVAL milliseconds later.
//this value can be changed via ascs::socket::msg_handling_interval(size_t) at runtime.

//#define ASCS_PASSIVE_RECV
//to gain the ability of changing the unpacker at runtime, with this macro, ascs will not do message receiving automatically (except the first one),
// so you need to manually call recv_msg(), if you need to change the unpacker, do it before recv_msg() invocation, please note.
//during async message receiving, calling recv_msg() will fail, this is by design to avoid asio::io_context using up all virtual memory.
//because user can greedily call recv_msg(), it's your responsibility to keep the recv buffer from overflowed, please pay special attention.
//this macro also makes you to be able to pause message receiving, then, if there's no other tasks (like timers), service_pump will stop itself,
// to avoid this, please define macro ASCS_AVOID_AUTO_STOP_SERVICE.

//#define ASCS_DISPATCH_BATCH_MSG
//all messages will be dispatched via on_handle_msg with a variable-length container, this will change the signature of function on_msg_handle,
//it's very useful if you want to re-dispatch message in your own logic or with very simple message handling (such as echo server).
//it's your responsibility to remove handled messages from the container (can be a part of them).

//#define ASCS_ALIGNED_TIMER
//for example, start a timer at xx:xx:xx, interval is 10 seconds, the callback will be called at (xx:xx:xx + 10), and suppose that the callback
//returned at (xx:xx:xx + 11), then the interval will be temporarily changed to 9 seconds to make the next callback to be called at (xx:xx:xx + 20),
//if you don't define this macro, the next callback will be called at (xx:xx:xx + 21), please note.

//#define ASCS_SYNC_SEND
#ifdef ASCS_SYNC_SEND
static_assert(ASIO_HAS_STD_FUTURE == 1, "sync message sending needs std::future.");
#endif
//#define ASCS_SYNC_RECV
//define these macro to gain additional series of sync message sending and receiving, they are:
// sync_send_msg
// sync_send_native_msg
// sync_safe_send_msg
// sync_safe_send_native_msg
// sync_recv_msg
//please note that:
// this feature will slightly impact efficiency even if you always use async message sending and receiving, so only open this feature when really needed.
// we must avoid to do sync message sending and receiving in service threads.
// if prior sync_recv_msg() not returned, the second sync_recv_msg() will return false immediately.
// with macro ASCS_PASSIVE_RECV, in sync_recv_msg(), recv_msg() will be automatically called.
// after returned from sync_recv_msg(), ascs will not maintain those messages any more.

//Sync operations are not tracked by tracked_executor, please note.
//Sync operations can be performed with async operations concurrently.
//If both sync message receiving and async message receiving exist, sync receiving has the priority no matter it was initiated before async receiving or not.

//#define ASCS_SYNC_DISPATCH
//with this macro, virtual size_t on_msg(std::list<OutMsgType>& msg_can) will be provided, you can rewrite it and handle all or a part of the
// messages like virtual function on_msg_handle (with macro ASCS_DISPATCH_BATCH_MSG), if your logic is simple enough (like echo or pingpong test),
// this feature is recommended because it can slightly improve efficiency.
//now we have three ways to handle messages (sync_recv_msg, on_msg and on_msg_handle), the invocation order is the same as listed, if messages been successfully
// dispatched to sync_recv_msg, then the second two will not be called, otherwise messages will be dispatched to on_msg, if on_msg only handled a part of (include
// zero) the messages, then on_msg_handle will continue to dispatch the rest of them asynchronously, this will disorder messages because on_msg_handle and the next
// on_msg (new messages arrived) can be invoked concurrently, please note. as before, on_msg will block the next receiving but only on current socket.
//if you cannot handle all of the messages in on_msg (like echo_server), you should not use sync message dispatching except you can bear message disordering.

//configurations

#endif /* _ASCS_CONFIG_H_ */