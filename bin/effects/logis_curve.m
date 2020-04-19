function logis_curve()

x = -100:100;
y = 1./(1+exp(x));

rate = 30;
[~,num] = size(y);
count = 0;

for i=1:num/rate:num
    value = y(ceil(i))*255.0;
    imwrite(value, ['D:\workspace\1.im2video\Bin\effects\ad\' num2str(count) 'A.jpg']);
    imwrite(255-value, ['D:\workspace\1.im2video\Bin\effects\ad\' num2str(count) 'B.jpg']);
    count = count + 1;
end

end