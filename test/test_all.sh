#!/bin/bash
echo "==== 长沙战区作战系统测试 ===="

# 编译测试版本
make clean
make test

# 执行路径规划测试
echo ""
echo ">> 路径规划测试"
./bin/bcs_test --test-path

# 执行导弹分配测试
echo ""
echo ">> 导弹分配测试"
./bin/bcs_test --test-missile

# 执行雷达系统测试
echo ""
echo ">> 雷达系统测试"
./bin/bcs_test --test-radar

echo ""
echo "==== 所有测试完成 ===="