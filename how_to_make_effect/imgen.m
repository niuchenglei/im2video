
%{
img = ones(1, 1);   %ones(256, 256);
for t=0:1
    alpha = 1;%t/30.0;
    imgA = img.*255;
    imgB = img.*255;
    imwrite(uint8(imgA), ['D:\workspace\1.im2video\Bin\effects\flyup\' int2str(t) 'A.jpg']);
    imwrite(uint8(imgB), ['D:\workspace\1.im2video\Bin\effects\flyup\' int2str(t) 'B.jpg']);
end
%}

%{
mov = mmreader('C:\Users\niuniu\Desktop\ad.wmv');
frames=read(mov, [301, 356]);  % Reads from frame 1 to frame 100
[r,c,n,fs] = size(frames);
for i=1:fs
    im = frames(:,:,:,i);
    im = rgb2gray(im);
    %im = imresize(im, [512, 512]);
    imwrite(im, ['C:\Users\niuniu\Desktop\a\' num2str(i+300) '.jpg']);
end

count = 1;
for i=28:7:323
    im = imread(['C:\Users\niuniu\Desktop\a\' num2str(i) '.jpg']);
    %im = rgb2gray(im);
    imwrite(im, ['C:\Users\niuniu\Desktop\b\' num2str(count) '.jpg']);
    count=count+1;
end

for i=1:43
    im = imread(['C:\Users\niuniu\Desktop\b\' num2str(i) '.jpg']);
    img = 255-im;
    imwrite(im, ['C:\Users\niuniu\Desktop\m\' num2str(i-1) 'B.jpg']);
    imwrite(img, ['C:\Users\niuniu\Desktop\m\' num2str(i-1) 'A.jpg']);
end
%}

%{
% circle
img = ones(700, 700);
sigma = 180;
x=-700/2:700/2;
y=-700/2:700/2;
for i=-700/2:700/2-1
    for j=-700/2:700/2-1
        p = i^2/2/sigma^2 + j^2/2/sigma^2;
        y = 1/2/pi/sigma/sigma*exp(-p);
        img(i+700/2+1, j+700/2+1) = y;
    end
end
max_v = max(max(img));
min_v = min(min(img));
count = 40;
step = (max_v-min_v)/count;
range = 1.0;
imwrite(ones(512,512)*255, ['C:\Users\niuniu\Desktop\aa\' num2str(0) 'A.jpg']);
imwrite(zeros(512,512)*255, ['C:\Users\niuniu\Desktop\aa\' num2str(0) 'B.jpg']);
for i=1:40
    thresh = min_v + (40-i+1)*step;
    im = img - thresh;
    im(im<0) = 0;
    im(im>0) = 255;
    
    h = fspecial('gaussian', [50 50], 80);
    im = imfilter(im, h);
    im = im(700/2-512/2+1:700/2+512/2, 700/2-512/2+1:700/2+512/2); 
    %imshow(im, []);
    imwrite(uint8(im), ['C:\Users\niuniu\Desktop\aa\' num2str(i) 'B.jpg']);
    imA = ones(512,512).*255 - im;
    imwrite(uint8(imA), ['C:\Users\niuniu\Desktop\aa\' num2str(i) 'A.jpg']);
end
%}

%{
%eraseout
img = ones(700, 700);
sigma = 150;
x=-700/2:700/2;
for i=-700/2:700/2-1
    if(i<0)
        v=1;
    else
        v=-1;
    end
    img(:, i+700/2+1) = v*255/700*2*i+255;
end
max_v = 255;
min_v = 0;
count = 40;
step = (max_v-min_v)/count;
range = 1.0;
for i=1:count
    thresh = min_v + (40-i+1)*step;
    im = img - thresh;
    im(im<0) = 0;
    im(im>0) = 255;
    
    %h = fspecial('gaussian', [50 50], 80);
    %im = imfilter(im, h);
    im = im(700/2-512/2+1:700/2+512/2, 700/2-512/2+1:700/2+512/2); 
    %imshow(im, []);
    imwrite(uint8(im(1,:)), ['C:\Users\niuniu\Desktop\aa\' num2str(i-1) 'B.jpg']);
    imA = ones(512,512).*255 - im;
    imwrite(uint8(imA(1,:)), ['C:\Users\niuniu\Desktop\aa\' num2str(i-1) 'A.jpg']);
end
%}

%{
%eraseleft
img = ones(700, 700);
sigma = 150;
x=-700/2:700/2;
for i=-700/2:700/2-1
    img(:, i+700/2+1) = i;
end
max_v = max(max(img));
min_v = min(min(img));
count = 40;
step = (max_v-min_v)/count;
range = 1.0;
cc = 0;
for i=1:count
    thresh = min_v + (40-i+1)*step;
    im = img - thresh;
    im(im<0) = 0;
    im(im>0) = 255;
    
    if(i<=5)
        continue;
    end
    
    %h = fspecial('gaussian', [50 50], 80);
    %im = imfilter(im, h);
    im = im(700/2-512/2+1:700/2+512/2, 700/2-512/2+1:700/2+512/2); 
    %imshow(im, []);
    imwrite(uint8(im(1,:)), ['C:\Users\niuniu\Desktop\aa\' num2str(cc) 'B.jpg']);
    imA = ones(512,512).*255 - im;
    imwrite(uint8(imA(1,:)), ['C:\Users\niuniu\Desktop\aa\' num2str(cc) 'A.jpg']);
    cc = cc+1;
end
%}

% randline
%{
mov = mmreader('C:\Users\niuniu\Desktop\123.wmv');
frames=read(mov, [1, 153]);  % Reads from frame 1 to frame 100
[r,c,n,fs] = size(frames);
for i=1:fs
    im = frames(:,:,:,i);
    im = rgb2gray(im);
    im = imresize(im, [512, 512]);
    imwrite(im, ['C:\Users\niuniu\Desktop\aa\' num2str(i-1) '.jpg']);
end
count = 0;
for i=20:3:115
    im = imread(['C:\Users\niuniu\Desktop\aa\' num2str(i) '.jpg']);
    imwrite(uint8(im(1,:)), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'B.jpg']);
    imwrite(uint8(255-im(1,:)), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'A.jpg']);
    count=count+1;
end
%}

%{
% shaperhombus
img = ones(725,725) + 255;
img = imrotate(img, 45, 'bilinear','loose');
img_canvas = zeros(1024,1024);
count = 75;
step = 725*sqrt(2)/count;
cc = 0;
for i=0:count
    ss = i*step;
    imv = img_canvas;
    if(ss ~= 0 && i ~= count)
        im = imresize(img,[ss ss]);
        [r,c] = size(im);
        imv(1024/2-r/2:1024/2+r/2-1, 1024/2-c/2:1024/2+c/2-1) = im;
    end
    imv = imv(1024/2-512/2:1024/2+512/2-1, 1024/2-512/2:1024/2+512/2-1);
    %imshow(imv, []);
    imwrite(uint8(imv), ['C:\Users\niuniu\Desktop\b\' num2str(cc) 'B.jpg']);
    imwrite(uint8(255-imv), ['C:\Users\niuniu\Desktop\b\' num2str(cc) 'A.jpg']);
    cc=cc+1;
end
%}

% shapeclock
%{
mov = mmreader('C:\Users\niuniu\Desktop\1239.wmv');
frames=read(mov, [1, 148]);  % Reads from frame 1 to frame 100
[r,c,n,fs] = size(frames);
for i=1:fs
    im = frames(:,:,:,i);
    im = rgb2gray(im);
    im = imresize(im, [512, 512]);
    imwrite(im, ['C:\Users\niuniu\Desktop\aa\' num2str(i-1) '.jpg']);
end
count = 0;
for i=16:1:116
    im = imread(['C:\Users\niuniu\Desktop\aa\' num2str(i) '.jpg']);
    imwrite(uint8(im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'B.jpg']);
    imwrite(uint8(255-im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'A.jpg']);
    count=count+1;
end
%}

% shapeclock
%{
mov = mmreader('C:\Users\niuniu\Desktop\1239.wmv');
frames=read(mov, [1, 148]);  % Reads from frame 1 to frame 100
[r,c,n,fs] = size(frames);
for i=1:fs
    im = frames(:,:,:,i);
    im = rgb2gray(im);
    im = imresize(im, [512, 512]);
    imwrite(im, ['C:\Users\niuniu\Desktop\aa\' num2str(i-1) '.jpg']);
end
count = 0;
for i=16:1:116
    im = imread(['C:\Users\niuniu\Desktop\aa\' num2str(i) '.jpg']);
    im(:,1:512/2) = fliplr(im(:,512/2+1:512));
    imwrite(uint8(im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'B.jpg']);
    imwrite(uint8(255-im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'A.jpg']);
    count=count+1;
end
for i=0:54
    ima = imread(['C:\Users\niuniu\Desktop\b\' num2str(i) 'A.jpg']);
    ima(1:512/2, 512/2) = ima(1:512/2, 512/2-1);
    ima(1:512/2, 512/2+1) = ima(1:512/2, 512/2+2);
    %imshow(ima,[]);
    imwrite(ima, ['C:\Users\niuniu\Desktop\b\' num2str(i) 'A.jpg']);
    imwrite(255-ima, ['C:\Users\niuniu\Desktop\b\' num2str(i) 'B.jpg']);
end
%}

% wave
mov = mmreader('D:\workspace\1.im2video\Bin\effect_demo\fadescale.avi');
frames=read(mov, [1, 65]);  % Reads from frame 1 to frame 100
[r,c,n,fs] = size(frames);
for i=1:fs
    im = frames(:,:,:,i);
    %im = rgb2gray(im);
    %im = imresize(im, [512, 512]);
    imwrite(im, ['C:\Users\niuniu\Desktop\app\' num2str(i-1) '.jpg']);
end
count = 0;
for i=16:1:116
    im = imread(['C:\Users\niuniu\Desktop\aa\' num2str(i) '.jpg']);
    imwrite(uint8(im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'B.jpg']);
    imwrite(uint8(255-im), ['C:\Users\niuniu\Desktop\b\' num2str(count) 'A.jpg']);
    count=count+1;
end

