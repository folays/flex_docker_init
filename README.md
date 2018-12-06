A minimalist /sbin/init for Docker, only reap childrens and propagate SIGTERM.

# Usage

Just put in your Dockerfile a `CMD` instruction:

```
ENTRYPOINT ["/sbin/flex-docker-init"]
CMD []
```

# Example

Your Dockerfile could looks like (using multi-stage builds):

```
FROM ubuntu:cosmic AS flex_docker_init

RUN apt-get update
RUN apt-get install -y git-core pmake gcc
RUN git clone https://github.com/folays/flex_docker_init && \
	 cd flex_docker_init/ && pmake
```

```
FROM ubuntu:cosmic

RUN apt-get update
COPY --from=flex_docker_init flex_docker_init/flex-docker-init /sbin/
COPY --from=flex_docker_init flex_docker_init/rc.local.flex-docker /etc/

ENTRYPOINT [“/sbin/flex-docker-init”]
CMD []

[...]
```

# /etc/rc.local.flex-docker

You might want to run a `rc file` (a shell script executed by `/bin/sh`).
Just create a `/etc/rc.local.flex-docker` file which should contains, for example:
> The script belows will be run by `/sbin/flex_docker_init` at startup.
>
> **If** the `CMD[]` instruction contains `--auto` (or `docker run IMAGE --auto`), then `/etc/rc.local.auto` will also be run
> (It helps start your container in "only start essentials things" or "start everything" mode)


```
#!/bin/sh

/usr/sbin/sshd
/usr/sbin/cron
[...]
daemon -- /whatever/binary/to/daemonize
[...]

params="$(getopt -s sh -o +a --longoptions auto -- "$@")" || exit 1
eval set -- "$params"

while true; do
    case "$1" in
        -a|--auto)
            auto=y
            shift 1
            ;;
        --)
            shift
            break
            ;;
    esac
done

if [ X"$auto" = X"y" ]; then
  echo AUTO
  [ -e "/etc/rc.local.auto" ] && /bin/sh /etc/rc.local.auto
fi

exit 0
```