import time
import torch

from diffusers import StableDiffusionPipeline
from diffusers import PNDMScheduler
from diffusers import DDPMScheduler
from diffusers import DDIMScheduler
from diffusers import EulerDiscreteScheduler
from diffusers import DEISMultistepScheduler
from diffusers import DPMSolverMultistepScheduler
from diffusers import KDPM2DiscreteScheduler
from diffusers import LMSDiscreteScheduler
from diffusers import EulerAncestralDiscreteScheduler
from diffusers import KDPM2AncestralDiscreteScheduler
from diffusers import HeunDiscreteScheduler
from diffusers import UniPCMultistepScheduler
from diffusers import DPMSolverSinglestepScheduler

from PIL import Image

def image_grid(imgs, rows, cols):
    assert len(imgs) == rows*cols

    w, h = imgs[0].size
    grid = Image.new('RGB', size=(cols*w, rows*h))
    grid_w, grid_h = grid.size

    for i, img in enumerate(imgs):
        grid.paste(img, box=(i%cols*w, i//cols*h))
    return grid

def time_csv(times, rows, cols):
    assert len(times) == rows*cols

    csv = ''
    for i in range(rows):
        for j in range(cols):
            csv += str(times[i*cols+j]) + ','
        csv += '\n'
    return csv

model_id = "gsdf/Counterfeit-V2.5"
device = "cuda"
pipeline = StableDiffusionPipeline.from_pretrained(model_id, torch_dtype=torch.float16)
pipeline = pipeline.to(device)

schedulers = [
        PNDMScheduler.from_config(pipeline.scheduler.config),
        DDPMScheduler.from_config(pipeline.scheduler.config),
        DDIMScheduler.from_config(pipeline.scheduler.config),
        EulerDiscreteScheduler.from_config(pipeline.scheduler.config),
        DEISMultistepScheduler.from_config(pipeline.scheduler.config),
        DPMSolverMultistepScheduler.from_config(pipeline.scheduler.config),     # looks best to me
        KDPM2DiscreteScheduler.from_config(pipeline.scheduler.config),
        LMSDiscreteScheduler.from_config(pipeline.scheduler.config),
        EulerAncestralDiscreteScheduler.from_config(pipeline.scheduler.config),
        KDPM2AncestralDiscreteScheduler.from_config(pipeline.scheduler.config),
        HeunDiscreteScheduler.from_config(pipeline.scheduler.config),
        UniPCMultistepScheduler.from_config(pipeline.scheduler.config),
        DPMSolverSinglestepScheduler.from_config(pipeline.scheduler.config),
        ]

prompt = "masterpiece,1girl,silver_hair,long_straight_hair,look_at_viewer,school_uniform,ribbon,hands,shoes,classroom"
n_prompt = "EasyNegative, extra fingers,fewer fingers"
scale = 7.5
steps = [20, 50]
seed = 1024
generator = torch.Generator(device).manual_seed(seed)

images = []
times = []
for step in steps:
    for scheduler in schedulers:
        pipeline.scheduler = scheduler
        start_time = time.perf_counter()
        image = pipeline(
                prompt=prompt,
                negative_prompt=n_prompt,
                guidance_scale=scale,
                num_inference_steps=step,
                generator=generator
                ).images[0]
        end_time = time.perf_counter()
        images.append(image)
        times.append(end_time - start_time)

grid = image_grid(images, len(steps), len(schedulers))
grid.save('result.png')

with open('result.txt', mode='w') as f:
    csv = time_csv(times, len(steps), len(schedulers))
    f.write(csv)

