import argparse
import asyncio
import itertools
import os
import sys
import vt


async def get_files_to_upload(queue, path):
    """Finds which files will be uploaded to VirusTotal."""
    if os.path.isfile(path):
        await queue.put(path)
        return 1

    n_files = 0
    with os.scandir(path) as it:
        for entry in it:
            if not entry.name.startswith(".") and entry.is_file():
                await queue.put(entry.path)
                n_files += 1
    return n_files


async def upload_hashes(queue, apikey):
    """Uploads selected files to VirusTotal."""
    return_values = []

    async with vt.Client(apikey) as client:
        while not queue.empty():
            file_path = await queue.get()
            if not os.path.exists(file_path):
                print(f"ERROR: file {file_path} not found.")
            with open(file_path, "rb") as f:
                analysis = await client.scan_file_async(file=f)
                print(f"File {file_path} uploaded.")
                queue.task_done()
                return_values.append((analysis, file_path))

    return return_values


async def process_analysis_results(apikey, analysis, file_path):
    async with vt.Client(apikey) as client:
        completed_analysis = await client.wait_for_analysis_completion(analysis)
        print(file_path + ":")
        print(f"   {completed_analysis.stats}")
        print(f"   id={completed_analysis.id}")


async def vt_check_dir(path: str, api_key: str):
    if not os.path.exists(path):
        print(f"ERROR: file {path} not found.")
        sys.exit(1)

    queue = asyncio.Queue()
    n_files = await get_files_to_upload(queue, path)

    worker_tasks = []
    for _ in range(min(4, n_files)):
        worker_tasks.append(asyncio.create_task(upload_hashes(queue, api_key)))

    analyses = itertools.chain.from_iterable(await asyncio.gather(*worker_tasks))
    await asyncio.gather(
        *[
            asyncio.create_task(process_analysis_results(api_key, a, f))
            for a, f in analyses
        ]
    )


async def vt_check_files(files: list, api_key: str):
    queue = asyncio.Queue()
    for f in files:
        await queue.put(f)

    n_files = len(files)

    worker_tasks = []
    for _ in range(min(4, n_files)):
        worker_tasks.append(asyncio.create_task(upload_hashes(queue, api_key)))

    analyses = itertools.chain.from_iterable(await asyncio.gather(*worker_tasks))
    await asyncio.gather(
        *[
            asyncio.create_task(process_analysis_results(api_key, a, f))
            for a, f in analyses
        ]
    )
