function erase_feature()
erase_ff();

end

function erase1()

pic = zeros(512, 512);
count = 0;
c = 0;
step = floor(512*2/29);

for i=1:512*2
    c = c + 1;
    if mod(i, 2) == 0
        pic(:, i/2) = 256;
    else
        pic(floor(i/2)+1, :) = 256;
    end
    if c == step
        count = count + 1;
        imwrite(uint8(pic), ['D:\workspace\1.im2video\Bin\effects\erase1\' int2str(count) 'B.jpg']);
        imwrite(uint8(255-pic), ['D:\workspace\1.im2video\Bin\effects\erase1\' int2str(count) 'A.jpg']);
        c = 0;
    end
end

end


function erase_cross()

pic = zeros(512, 512);
count = 0;
c = 0;
step = floor(512/2/29);

for i=1:512/2-1
    c = c + 1;
    
    pic(512/2-i:512/2+i+1, :) = 256;
    pic(:,512/2-i:512/2+i+1) = 256;
    
    if c == step
        count = count + 1;
        imwrite(uint8(pic), ['D:\workspace\1.im2video\Bin\effects\cross\' int2str(count) 'B.jpg']);
        imwrite(uint8(255-pic), ['D:\workspace\1.im2video\Bin\effects\cross\' int2str(count) 'A.jpg']);
        c = 0;
    end
end
end

function erase_ff()

pic = ones(1, 512)*255;

for i=0:14
    imwrite(uint8(pic), ['D:\workspace\1.im2video\Bin\effects\shuffle\' int2str(i) 'B.jpg']);
    imwrite(uint8(pic), ['D:\workspace\1.im2video\Bin\effects\shuffle\' int2str(i) 'A.jpg']);
end

c = 0;
count = 15;
step = floor(512/15);
for i=1:512
    c = c + 1;
    
    pic(:, i) = 0;
    
    if c == step
        imwrite(uint8(pic), ['D:\workspace\1.im2video\Bin\effects\shuffle\' int2str(count) 'A.jpg']);
        imwrite(uint8(ones(1, 512)*255), ['D:\workspace\1.im2video\Bin\effects\shuffle\' int2str(count) 'B.jpg']);
        count = count + 1;
        c = 0;
    end
end
end
