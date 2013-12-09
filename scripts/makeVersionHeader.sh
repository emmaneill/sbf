#!/bin/bash

description=$(git describe)
version=${description%%-*}
read a b c <<<${version//./ }

cat <<EOF >common/sbfVersion.h
#ifndef _SBF_VERSION_H_
#define _SBF_VERSION_H_

#define SBF_MAJOR_VERSION $a
#define SBF_MINOR_VERSION $b
#define SBF_PATCH_LEVEL $c

#define SBF_VERSION "$version"

#endif /* _SBF_VERSION_H_ */
EOF