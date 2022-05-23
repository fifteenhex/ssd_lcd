
set $last_mod_head = modules.prev
set $curr_mod_head = modules.next

  printf "\n\n Load Address(size)\t\t Module Name\n"
  printf "=======================================================\n"

while($curr_mod_head!=$last_mod_head)
  set $cur_mod = ((struct module*)((unsigned int)$curr_mod_head-0x4))
  printf "@ 0x%08x(%d) \t\t %s\n",($cur_mod->core_layout.base),($cur_mod->core_layout.size),($cur_mod->name)
  set $curr_mod_head = $curr_mod_head.next
end
