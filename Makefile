all : simple_tcp fifo_fork fifo_no_rela_c fifo_no_rela_s pipe2process pipe_full_duplex ppopen \
	resource fifo_1ton_s fifo_1ton_c

simple_tcp : simple_tcp.cpp
	g++ $< -o $@

fifo_fork : fifo_fork.cpp
	g++ $< -o $@

fifo_no_rela_c : fifo_no_relation_client.cpp
	g++ $< -o $@

fifo_no_rela_s : fifo_no_relation_server.cpp
	g++ $< -o $@

pipe2process : pipe_between_two_process.cpp
	g++ $< -o $@

pipe_full_duplex : pipe_full_duplex.cpp
	g++ $< -o $@

ppopen : pipe_popen.cpp
	g++ $< -o $@

resource : resource.cpp
	g++ $< -o $@

fifo_1ton_s : fifo_single_server_multi_client_s.cpp
	g++ $< -o $@

fifo_1ton_c : fifo_single_server_multi_client_c.cpp
	g++ $< -o $@