WATCHDOG_DIR = src/watchdog
PHANTOM_DIR  = src/phantom

.PHONY: all build install demo clean

all: build

build:
	$(MAKE) -C $(WATCHDOG_DIR)
	$(MAKE) -C $(PHANTOM_DIR)

install: build
	sudo bash deploy/install.sh

demo:
	bash scripts/start-demo.sh

clean:
	$(MAKE) -C $(WATCHDOG_DIR) clean
	$(MAKE) -C $(PHANTOM_DIR) clean
