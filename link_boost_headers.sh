#!/bin/sh

if [ $# != 1 ] ; then 
    echo "USAGE: $0 SRC_BOOST_DIR" 
    echo " e.g.: $0 boost_1_58_0"
    exit 1; 
fi 

CURR_DIR=$PWD
SELF_NAME=`basename $0`
SRC_BOOST_DIR=$1

function link_header_files
{
    cd $SRC_BOOST_DIR
    header_files=`find boost -name "*.h" -o -name "*.hpp" -o -name "*.ipp"`
    for file_name in $header_files; do
        echo $file_name
        header_path=thirdparty/$SRC_BOOST_DIR/$file_name
        header_macro=`echo THIRDPARTY_$file_name | tr "/." "_" | tr "[:lower:]" "[:upper:]"`
        link_file=$CURR_DIR/$file_name
        mkdir -p `dirname $link_file`
        echo "#include \"$header_path\"" >> $link_file
    done
}

function gen_build
{
    cd $CURR_DIR
    echo "boost/BUILD"
    echo "cc_library(" >boost/BUILD
    echo "    name = 'system'," >>boost/BUILD
    echo "    deps = ['//thirdparty/$SRC_BOOST_DIR:system']," >>boost/BUILD
    echo ")" >>boost/BUILD
}

rm -rf boost
link_header_files
gen_build

