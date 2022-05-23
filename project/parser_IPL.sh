temp=$(cat $1|grep -a "MVX")
echo setenv ipl_version \""${temp##*MVX5}"\" >> $2/set_config