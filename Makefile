all: release

clean:
	    rm -rf build

debug:
	    mkdir -p build/debug && \
		cd build/debug && \
		cmake -DCMAKE_BUILD_TYPE=Debug ../.. && \
		cmake --build .

release:
	    mkdir -p build/release && \
		cd build/release && \
		cmake -DCMAKE_BUILD_TYPE=Release ../.. && \
		cmake --build .

